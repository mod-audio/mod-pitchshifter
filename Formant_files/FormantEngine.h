#ifndef MOD_PITCHSHIFTER_FORMANTENGINE_H
#define MOD_PITCHSHIFTER_FORMANTENGINE_H

/* Phase-vocoder pitch shifter with formant correction, for the formant-preserving Capo and Drop.
 *
 * Same structure as PSAnalysis/PSSinthesis (time-stretch by the pitch ratio, then linear resampling
 * back to the hop), with three changes:
 *  - exact phase (atan2, sin/cos) instead of the approximations;
 *  - identity phase locking (Laroche & Dolson 1999): the bins around each spectral peak keep their
 *    analysis phase offset to the peak, so partials stay coherent;
 *  - formant correction: the spectral envelope ("true envelope", Röbel: iterated cepstral smoothing
 *    that rides the harmonic peaks) is put back where it was before the shift, so body resonances
 *    (guitar) or vowels (voice) do not move with the pitch.
 *
 * The lifter cut nc (in cepstrum samples) sets how fine the envelope is: nc = fs / (2 f0) follows the
 * harmonics of a note at f0. The guitar plugins use a fixed f0 (chords); a voice plugin would pass a
 * detected pitch.
 *
 * Header-only on purpose: the existing plugins compile every .cpp in Shared_files, and must stay as
 * they are. Everything is allocated and planned in the constructor; Reset/Push/Process are real-time
 * safe. */

#include <stdint.h>
#include <string.h>
#include <cmath>
#include <algorithm>
#include <fftw3.h>
#include "Wisdom.h"

class FormantEngine
{
public:
	FormantEngine(int hop, int nBuffers, bool haveWisdom)
		: N(nBuffers * hop), hop(hop), Q(nBuffers), K(N / 2 + 1)
	{
		frames  = new double[N]();
		w       = new double[N];
		for (int i = 0; i < N; i++)
			w[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / (N - 1)));
		prevArg = new double[K]();
		phi     = new double[K]();
		arg     = new double[K]();
		mag     = new double[K]();
		omega   = new double[K]();
		newPhi  = new double[K]();
		env     = new float[K]();
		peaks   = new int[K];
		hops    = new int[Q];
		ysLen   = 2 * N + 4 * (Q - 1) * hop + 8;
		ys      = new double[ysLen]();
		yshift  = new double[hop]();

		tin  = fftwf_alloc_real(N);
		spec = fftwf_alloc_complex(K);
		tout = fftwf_alloc_real(N);
		la   = new float[K];
		ecx  = fftwf_alloc_complex(K);
		ecep = fftwf_alloc_real(N);
		ecx2 = fftwf_alloc_complex(K);

		pa = PlanR2C(N, tin, spec, haveWisdom);
		ps = PlanC2R(N, spec, tout, haveWisdom);
		// envelope: log spectrum -> cepstrum -> lifter -> back, with the same transform sizes as the engine
		// (r2r DCT-I of N/2+1 would be cheaper on paper, but FFTW allocates while running it)
		pc = PlanC2R(N, ecx, ecep, haveWisdom);
		pe = PlanR2C(N, ecep, ecx2, haveWisdom);

		Reset();
	}

	~FormantEngine()
	{
		if (pa) fftwf_destroy_plan(pa);
		if (ps) fftwf_destroy_plan(ps);
		if (pc) fftwf_destroy_plan(pc);
		if (pe) fftwf_destroy_plan(pe);
		fftwf_free(tin); fftwf_free(spec); fftwf_free(tout);
		fftwf_free(ecx); fftwf_free(ecep); fftwf_free(ecx2); delete[] la;
		delete[] frames; delete[] w; delete[] prevArg; delete[] phi; delete[] arg; delete[] mag;
		delete[] omega; delete[] newPhi; delete[] env; delete[] peaks; delete[] hops; delete[] ys;
		delete[] yshift;
	}

	// clear the history; real-time safe
	void Reset()
	{
		std::fill_n(frames, N, 0.0);
		std::fill_n(prevArg, K, 0.0);
		std::fill_n(phi, K, 0.0);
		std::fill_n(hops, Q, hop);
		std::fill_n(ys, ysLen, 0.0);
		std::fill_n(yshift, hop, 0.0);
		std::fill_n(env, K, 0.0f);
		first = true;
		envCount = 0;
	}

	// one hop of input in: slide the frame and the hop history (PreAnalysis + PreSinthesis)
	void Push(const float *in)
	{
		memmove(frames, frames + hop, sizeof(double) * (N - hop));
		for (int i = 0; i < hop; i++)
			frames[N - hop + i] = in[i];
		for (int k = 0; k < Q - 1; k++)
			hops[k] = hops[k + 1];
	}

	/* Analysis + synthesis of the current frame; the hop's output is in yshift.
	 * semis: shift in semitones; nc: lifter cut (cepstrum samples), used when formant is true. */
	void Process(double semis, int nc, bool formant)
	{
		const double alpha = pow(2.0, semis / 12.0);

		// ---- analysis
		const double norm = sqrt(N / (2.0 * hop));
		for (int i = 0; i < N; i++)
			tin[i] = frames[i] * w[i] / norm;
		fftwf_execute(pa);

		for (int k = 0; k < K; k++)
		{
			const float re = spec[k][0], im = spec[k][1];
			arg[k] = atan2f(im, re);
			mag[k] = sqrtf(re * re + im * im);
			double d = arg[k] - prevArg[k] - (2.0 * M_PI * hop / N) * k;
			d -= floor((d + M_PI) / (2.0 * M_PI)) * 2.0 * M_PI;
			omega[k] = (2.0 * M_PI / N) * k + d / hop;
			prevArg[k] = arg[k];
		}

		// ---- synthesis phase: advance by the synthesis hop, locked to the peaks
		const int hs = (int)lround(hop * alpha);
		hops[Q - 1] = hs;

		for (int k = 0; k < K; k++)
		{
			// keep the running phase wrapped: it is used in single precision below
			const double p = phi[k] + hs * omega[k];
			newPhi[k] = p - 2.0 * M_PI * floor(p / (2.0 * M_PI) + 0.5);
		}

		int np = 0;
		for (int k = 1; k < K - 1; k++)
			if (mag[k] > mag[k - 1] && mag[k] >= mag[k + 1])
				peaks[np++] = k;

		if (np > 0)
		{
			// each bin belongs to its nearest peak (boundaries half-way between peaks)
			int lo = 0;
			for (int p = 0; p < np; p++)
			{
				const int pk = peaks[p];
				const int hi = (p + 1 < np) ? (pk + peaks[p + 1]) / 2 + 1 : K;
				const double base = newPhi[pk] - arg[pk];
				for (int k = lo; k < hi; k++)
					if (k != pk)
						newPhi[k] = base + arg[k];
				lo = hi;
			}
		}
		std::copy(newPhi, newPhi + K, phi);

		// ---- magnitude: put the envelope back where it was before the shift
		if (formant && (envCount++ % envEvery) == 0)
		{
			if (envMode == 0)
				Envelope(std::max(8, std::min(nc, K - 2)));
			else
				PeakEnvelope(std::max(8, std::min(nc, K - 2)));
		}

		for (int k = 0; k < K; k++)
		{
			double m = mag[k];
			if (formant)
			{
				// a component that ends at k*alpha after the shift should carry env(k*alpha), not env(k)
				const double src = k * alpha;
				double e;
				if (src >= K - 1)
					e = env[K - 1];
				else
				{
					const int i0 = (int)src;
					const double f = src - i0;
					e = env[i0] + (env[i0 + 1] - env[i0]) * f;
				}
				m *= expf((float)(e - env[k]));
			}
			float sn, cs;
			sincosf((float)phi[k], &sn, &cs);
			spec[k][0] = m * cs;
			spec[k][1] = m * sn;
		}

		fftwf_execute(ps);

		// ---- overlap-add of the time-stretched signal, then resample one hop
		int L = N;
		for (int i = 0; i < Q - 1; i++)
			L += hops[i];
		const int base = L - N;
		const double qn = N * sqrt(N / (2.0 * hs));

		if (first)
		{
			first = false;
			std::fill_n(ys, base, 0.0);
			for (int i = 0; i < N; i++)
				ys[base + i] = tout[i] * w[i] / qn;
		}
		else
		{
			for (int i = 0; i < N; i++)
				ys[base + i] += tout[i] * w[i] / qn;
		}

		const double r = hs / (double)hop;
		const double *seg = ys + base;
		for (int n = 0; n < hop; n++)
		{
			const double p = n * r + 1;
			const int n1 = (int)floor(p), n2 = (int)ceil(p);
			yshift[n] = seg[n1] + (seg[n2] - seg[n1]) * (p - n1);
		}

		const int h0 = hops[0];
		memmove(ys, ys + h0, sizeof(double) * (L - h0));
		std::fill_n(ys + L - h0, h0, 0.0);
	}

	int envIters = 8; // true-envelope iterations
	int envEvery = 1; // compute the envelope every this many hops (it moves slowly)
	int envMode = 0;  // 0 = true envelope (cepstral), 1 = running max + smoothing (no FFT)
	float envMaxW = 1.0f, envSmoothW = 1.0f; // mode 1: widths in harmonic spacings

	const int N;    // frame size
	const int hop;  // analysis hop
	const int Q;    // frames in the overlap-add (N / hop)
	double *yshift; // the hop's output

private:
	// true envelope of mag (natural log), cut nc; result in env
	void Envelope(int nc)
	{
		for (int k = 0; k < K; k++)
			la[k] = logf(std::max((float)mag[k], 1e-9f));
		const float scale = 1.0f / N;
		for (int it = 0; it < envIters; it++)
		{
			for (int k = 0; k < K; k++)
			{
				ecx[k][0] = la[k];
				ecx[k][1] = 0.0f;
			}
			fftwf_execute(pc);                          // ecep = N * real cepstrum
			for (int q = 0; q < nc; q++)
				ecep[q] *= scale;
			std::fill(ecep + nc, ecep + N - nc + 1, 0.0f); // lifter (keep |quefrency| < nc)
			for (int q = N - nc + 1; q < N; q++)
				ecep[q] *= scale;
			fftwf_execute(pe);                          // ecx2 = smoothed log spectrum (real part)
			for (int k = 0; k < K; k++)
			{
				env[k] = ecx2[k][0];
				la[k] = std::max(la[k], env[k]);
			}
		}
	}

	/* Cheap envelope: the log spectrum's running maximum over about one harmonic spacing (rides the
	 * peaks like the true envelope does), then two passes of a moving average to smooth it. */
	void PeakEnvelope(int nc)
	{
		const float spacing = N / (2.0f * nc);   // harmonic spacing (bins) of the f0 behind nc
		const int hm = std::max(1, (int)lroundf(envMaxW * spacing * 0.5f));
		const int hs = std::max(1, (int)lroundf(envSmoothW * spacing * 0.5f));
		for (int k = 0; k < K; k++)
			la[k] = logf(std::max((float)mag[k], 1e-9f));
		for (int k = 0; k < K; k++)
		{
			float m = la[k];
			const int a = std::max(0, k - hm), b = std::min(K - 1, k + hm);
			for (int j = a; j <= b; j++)
				m = std::max(m, la[j]);
			env[k] = m;
		}
		for (int pass = 0; pass < 2; pass++)
		{
			// moving average of width 2*hs+1 via a prefix sum (ecep is free here: N >= K+1)
			float *ps = ecep;
			ps[0] = 0.0f;
			for (int k = 0; k < K; k++)
				ps[k + 1] = ps[k] + env[k];
			for (int k = 0; k < K; k++)
			{
				const int a = std::max(0, k - hs), b = std::min(K - 1, k + hs);
				env[k] = (ps[b + 1] - ps[a]) / (b - a + 1);
			}
		}
	}

	const int K;
	int ysLen;
	int envCount;
	bool first;
	double *frames, *w, *prevArg, *phi, *arg, *mag, *omega, *newPhi, *ys;
	float *env;
	int *peaks, *hops;
	float *tin, *tout, *la, *ecep;
	fftwf_complex *spec, *ecx, *ecx2;
	fftwf_plan pa, ps, pc, pe;
};

#endif
