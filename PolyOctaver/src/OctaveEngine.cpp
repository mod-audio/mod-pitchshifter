#include "OctaveEngine.h"
#include "Wisdom.h"

#include <algorithm>
#include <cmath>
#include <cstring>

static const float kRatio[OctaveEngine::NVOICES] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
static const int kBranches[OctaveEngine::NVOICES] = { 4, 2, 1, 1, 1 };

static const float kTwoPi = 6.28318530717958647692f;

static inline float wrapPhase(float x)
{
	return x - kTwoPi * std::floor(x / kTwoPi + 0.5f);
}

OctaveEngine::Config OctaveEngine::DefaultConfig(double sampleRate)
{
	const int scale = sampleRate > 140000.0 ? 4 : sampleRate > 70000.0 ? 2 : 1;
	const int frame = 4096 * scale;
	// Nothing over 8 kHz is worth an octave voice, and leaving it out is a third of the work.
	Config c = { frame, 768 * scale, 256 * scale, 24, static_cast<int>(8000.0 * frame / sampleRate + 0.5) };
	return c;
}

OctaveEngine::OctaveEngine(const Config& config, bool haveWisdom)
	: N(config.frame), M(config.synth), H(config.hop), T(config.hop / 2), NB(config.frame / 2 + 1), K(config.skirt),
	  LB(config.band > 2 && config.band < config.frame / 2 - 2 ? config.band : config.frame / 2 - 2)
{
	ring = fftwf_alloc_real(N);
	winA = fftwf_alloc_real(N);
	winS = fftwf_alloc_real(M);
	timeIn = fftwf_alloc_real(N);
	timeOut = fftwf_alloc_real(N);
	ola = fftwf_alloc_real(M);
	pending = fftwf_alloc_real(H);
	X = fftwf_alloc_complex(NB);
	Y = fftwf_alloc_complex(NB);
	refRe = fftwf_alloc_real(NB);
	refIm = fftwf_alloc_real(NB);
	mag2 = fftwf_alloc_real(NB);

	const int maxPeaks = NB / 2 + 1;
	candidate = new int[maxPeaks];
	explained = new float[NB];
	accepted = new char[NB];
	ampRe = new float[maxPeaks];
	ampIm = new float[maxPeaks];
	peakBin = new int[maxPeaks];
	regionLo = new int[maxPeaks];
	regionHi = new int[maxPeaks];
	peakPos = new float[maxPeaks];
	peakPhase = new float[maxPeaks];
	lastPhase[0] = new float[NB];
	lastPhase[1] = new float[NB];
	for (int v = 0; v < NVOICES; ++v) drift[v] = new float[NB];

	// Windows. Their product over the last M samples of the frame is what gets overlap-added.
	double* const product = new double[M];
	if (M == N)
	{
		for (int n = 0; n < N; ++n)
		{
			const double h = 0.5 - 0.5 * std::cos(2.0 * M_PI * n / N);
			winA[n] = static_cast<float>(h);
			product[n] = h * h;
		}
	}
	else
	{
		// Long rise up to the centre of the synthesis window, short fall after it; the synthesis
		// window is whatever turns that into a Hann window of length M.
		const int top = N - M / 2;
		for (int n = 0; n < N; ++n)
		{
			double a;
			if (n < top)
				a = 0.5 - 0.5 * std::cos(M_PI * n / top);
			else
				a = std::sin(M_PI * (n - (N - M)) / M);
			winA[n] = static_cast<float>(a);
		}
		for (int m = 0; m < M; ++m)
			product[m] = 0.5 - 0.5 * std::cos(2.0 * M_PI * m / M);
	}

	double olaGain = 0; // sum of the overlapping products, the same at every output sample
	for (int m = 0; m < M; m += H)
		olaGain += product[m];

	for (int m = 0; m < M; ++m)
	{
		const double a = winA[N - M + m];
		const double s = a > 1e-9 ? product[m] / a : 0.0;
		winS[m] = static_cast<float>(s / (olaGain * N)); // FFTW's inverse is unnormalised
	}

	// Phases are referred to the centre of the synthesis window: moving a region by a fraction of a
	// bin then costs the least there, where the output is taken from.
	const int centre = N - M / 2;
	for (int k = 0; k < NB; ++k)
	{
		const double a = 2.0 * M_PI * ((static_cast<long long>(k) * centre) % N) / N;
		refRe[k] = static_cast<float>(std::cos(a));
		refIm[k] = static_cast<float>(std::sin(a));
	}

	// Linear interpolation between two bins is, in time, the frame multiplied by
	// (1-f) + f e^{jx} instead of e^{jfx}, x = 2 pi (n - centre) / N. What that does to a steady
	// partial after the overlap-add, per fraction f, inverted.
	double productSum = 0;
	for (int m = 0; m < M; ++m) productSum += product[m];
	for (int i = 0; i <= kFracSteps; ++i)
	{
		const double f = static_cast<double>(i) / kFracSteps;
		double re = 0, im = 0;
		for (int m = 0; m < M; ++m)
		{
			const double x = 2.0 * M_PI * (N - M + m - centre) / N;
			const double er = (1.0 - f) + f * std::cos(x), ei = f * std::sin(x);
			const double cr = std::cos(f * x), ci = -std::sin(f * x);
			re += product[m] * (er * cr - ei * ci);
			im += product[m] * (er * ci + ei * cr);
		}
		re /= productSum; im /= productSum;
		const double d = re * re + im * im;
		fracRe[i] = static_cast<float>(re / d);
		fracIm[i] = static_cast<float>(-im / d);
	}
	delete[] product;

	lobeRe = lobeIm = lobeMag = 0;
	if (K > 0) BuildLobeTable();

	fwd = PlanR2C(N, timeIn, X, haveWisdom);
	inv = PlanC2R(N, Y, timeOut, haveWisdom);

	Reset();
}

// The window's transform on a grid of kLobeSteps points per bin: one FFT of the window, zero-padded.
void OctaveEngine::BuildLobeTable()
{
	const int points = (K + 1) * kLobeSteps + 1;
	lobeRe = new float[points];
	lobeIm = new float[points];
	lobeMag = new float[points];

	const int padded = N * kLobeSteps;
	float* const t = fftwf_alloc_real(padded);
	fftwf_complex* const f = fftwf_alloc_complex(padded / 2 + 1);
	const fftwf_plan plan = fftwf_plan_dft_r2c_1d(padded, t, f, FFTW_ESTIMATE);
	std::memset(t, 0, sizeof(float) * padded);
	std::memcpy(t, winA, sizeof(float) * N);
	if (plan) fftwf_execute(plan);

	const int centre = N - M / 2;
	for (int i = 0; i < points; ++i)
	{
		const double a = 2.0 * M_PI * std::fmod(static_cast<double>(i) * centre / kLobeSteps, static_cast<double>(N)) / N;
		const double c = std::cos(a), sn = std::sin(a);
		const double re = plan ? f[i][0] : 0.0, im = plan ? f[i][1] : 0.0;
		lobeRe[i] = static_cast<float>(re * c - im * sn);
		lobeIm[i] = static_cast<float>(re * sn + im * c);
		lobeMag[i] = std::sqrt(lobeRe[i] * lobeRe[i] + lobeIm[i] * lobeIm[i]);
	}

	if (plan) fftwf_destroy_plan(plan);
	fftwf_free(f);
	fftwf_free(t);

	// log(right neighbour / left neighbour) for a peak at every offset from its bin: the right one
	// is 1 - offset away, the left one 1 + offset
	for (int i = 0; i <= kOffsetSteps; ++i)
	{
		const float offset = -0.5f + static_cast<float>(i) / kOffsetSteps;
		offsetRatio[i] = std::log(LobeMagnitude(1.0f - offset) / LobeMagnitude(1.0f + offset));
	}
}

OctaveEngine::~OctaveEngine()
{
	if (fwd) fftwf_destroy_plan(fwd);
	if (inv) fftwf_destroy_plan(inv);
	for (int v = 0; v < NVOICES; ++v) delete[] drift[v];
	delete[] lastPhase[1];
	delete[] lastPhase[0];
	delete[] peakPhase;
	delete[] peakPos;
	delete[] regionHi;
	delete[] regionLo;
	delete[] peakBin;
	delete[] ampIm;
	delete[] ampRe;
	delete[] accepted;
	delete[] explained;
	delete[] candidate;
	delete[] lobeMag;
	delete[] lobeIm;
	delete[] lobeRe;
	fftwf_free(mag2);
	fftwf_free(refIm);
	fftwf_free(refRe);
	fftwf_free(Y);
	fftwf_free(X);
	fftwf_free(pending);
	fftwf_free(ola);
	fftwf_free(timeOut);
	fftwf_free(timeIn);
	fftwf_free(winS);
	fftwf_free(winA);
	fftwf_free(ring);
}

void OctaveEngine::Reset()
{
	std::memset(ring, 0, sizeof(float) * N);
	std::memset(ola, 0, sizeof(float) * M);
	std::memset(pending, 0, sizeof(float) * H);
	std::memset(lastPhase[0], 0, sizeof(float) * NB);
	std::memset(lastPhase[1], 0, sizeof(float) * NB);
	for (int v = 0; v < NVOICES; ++v) std::memset(drift[v], 0, sizeof(float) * NB);
	nPeaks = 0;
	ringAt = 0;
	silentTicks = N / T;
	analysed = false;
	synthesisNext = false;
}

void OctaveEngine::FindPeaks()
{
	float top = 0;
	for (int k = 0; k <= LB + 2; ++k)
	{
		// refer the phase to the centre of the synthesis window while we are here
		const float re = X[k][0] * refRe[k] - X[k][1] * refIm[k];
		const float im = X[k][0] * refIm[k] + X[k][1] * refRe[k];
		X[k][0] = re;
		X[k][1] = im;
		mag2[k] = re * re + im * im;
		if (mag2[k] > top) top = mag2[k];
	}

	// -70 dB under the strongest bin, and never under about -100 dBFS
	const float absolute = 1e-10f * (0.0625f * N) * N;
	const float threshold = top * 1e-7f > absolute ? top * 1e-7f : absolute;

	nCandidates = 0;
	for (int k = 2; k <= LB; ++k)
	{
		const float m = mag2[k];
		// With the skirt model a bare local maximum will do, ripples are rejected below; without it,
		// ask for a maximum over two bins either side.
		const bool wide = K > 0 || (m > mag2[k - 2] && m >= mag2[k + 2]);
		if (m > threshold && m > mag2[k - 1] && m >= mag2[k + 1] && wide)
		{
			candidate[nCandidates++] = k;
			++k; // the next bin cannot be a peak
		}
	}

	if (K > 0)
	{
		// The window's skirt ripples, and a ripple next to a strong partial is a local maximum too.
		// Strongest first: a candidate counts only if it stands 3 dB over what the partials accepted
		// so far already explain at its bin.
		const float* const strength = mag2;
		std::sort(candidate, candidate + nCandidates, [strength](int a, int b) { return strength[a] > strength[b]; });
		std::memset(explained, 0, sizeof(float) * (LB + 1));
		std::memset(accepted, 0, LB + 1);
		nPeaks = 0;

		for (int c = 0; c < nCandidates; ++c)
		{
			const int p = candidate[c];
			const float m = std::sqrt(mag2[p]);
			if (m <= 1.41f * explained[p])
				continue;
			accepted[p] = 1;
			if (++nPeaks == kMaxPartials)
				break; // noise has a peak every other bin: the cost has to stay bounded

			const float pos = p + PeakOffset(p);
			const float amp = m / lobeMag[static_cast<int>(std::fabs(p - pos) * kLobeSteps + 0.5f)];
			const int from = p - K < 1 ? 1 : p - K, to = p + K > LB ? LB : p + K;
			for (int k = from; k <= to; ++k)
			{
				const int idx = static_cast<int>(std::fabs(k - pos) * kLobeSteps + 0.5f);
				if (idx <= (K + 1) * kLobeSteps) explained[k] += amp * lobeMag[idx];
			}
		}

		nPeaks = 0;
		for (int k = 2; k <= LB; ++k)
			if (accepted[k]) peakBin[nPeaks++] = k;
	}
	else
	{
		nPeaks = nCandidates;
		std::memcpy(peakBin, candidate, sizeof(int) * nCandidates);
	}

	for (int i = 0; i < nPeaks; ++i)
	{
		const int p = peakBin[i];

		// region: up to the weakest bin between this peak and the next
		regionLo[i] = i == 0 ? 1 : regionHi[i - 1] + 1;
		if (i == nPeaks - 1)
			regionHi[i] = LB;
		else
		{
			int lowest = p + 1;
			for (int k = p + 2; k < peakBin[i + 1]; ++k)
				if (mag2[k] < mag2[lowest]) lowest = k;
			regionHi[i] = lowest;
		}

		peakPos[i] = p + PeakOffset(p);
		peakPhase[i] = std::atan2(X[p][1], X[p][0]);
	}
}

// Where between the bins the peak is: parabola through the log magnitudes of the peak and its neighbours.
float OctaveEngine::PeakOffset(int p) const
{
	const float a = std::log(mag2[p - 1] + 1e-30f), b = std::log(mag2[p] + 1e-30f), c = std::log(mag2[p + 1] + 1e-30f);
	const float curve = a - 2.0f * b + c;
	const float delta = curve < -1e-12f ? 0.5f * (a - c) / curve : 0.0f;
	return delta > 0.5f ? 0.5f : delta < -0.5f ? -0.5f : delta;
}

// What the other partials' current estimates put into bin `bin`, removed from it.
void OctaveEngine::Cleaned(int i, int bin, float& re, float& im) const
{
	re = X[bin][0];
	im = X[bin][1];
	for (int j = i - 1; j >= 0 && bin - peakPos[j] <= K; --j)
	{
		float wr, wi;
		Lobe(bin - peakPos[j], wr, wi);
		re -= ampRe[j] * wr - ampIm[j] * wi;
		im -= ampRe[j] * wi + ampIm[j] * wr;
	}
	for (int j = i + 1; j < nPeaks && peakPos[j] - bin <= K; ++j)
	{
		float wr, wi;
		Lobe(bin - peakPos[j], wr, wi);
		re -= ampRe[j] * wr - ampIm[j] * wi;
		im -= ampRe[j] * wi + ampIm[j] * wr;
	}
}

// Offset of a peak from its bin, from how its two neighbours compare: the window's own lobe tells
// what ratio every offset gives. The voices multiply any frame-to-frame error in this by their
// ratio, and with the synthesis at the far end of the analysis window a hundredth of a bin is
// already a degree of phase, so the parabola alone is not good enough.
float OctaveEngine::OffsetFromNeighbours(float left, float right) const
{
	const float wanted = std::log((right + 1e-30f) / (left + 1e-30f));
	if (wanted <= offsetRatio[0]) return -0.5f;
	if (wanted >= offsetRatio[kOffsetSteps]) return 0.5f;
	int lo = 0, hi = kOffsetSteps; // offsetRatio rises with the offset
	while (hi - lo > 1)
	{
		const int mid = (lo + hi) / 2;
		if (offsetRatio[mid] <= wanted) lo = mid; else hi = mid;
	}
	const float f = (wanted - offsetRatio[lo]) / (offsetRatio[hi] - offsetRatio[lo] + 1e-30f);
	return -0.5f + (lo + f) / kOffsetSteps;
}

// Complex amplitude and position of every partial, then the partials, skirts included, out of the spectrum.
void OctaveEngine::ModelPartials()
{
	for (int i = 0; i < nPeaks; ++i)
	{
		ampRe[i] = 0;
		ampIm[i] = 0;
	}

	// Each peak bin also holds its neighbours' skirts: a few rounds of solving every partial with
	// the others' current estimates taken out.
	for (int round = 0; round < 4; ++round)
		for (int i = 0; i < nPeaks; ++i)
		{
			const int p = peakBin[i];
			float re, im, lr, li, rr, ri;
			Cleaned(i, p, re, im);
			if (round > 0)
			{
				Cleaned(i, p - 1, lr, li);
				Cleaned(i, p + 1, rr, ri);
				peakPos[i] = p + OffsetFromNeighbours(std::sqrt(lr * lr + li * li), std::sqrt(rr * rr + ri * ri));
			}
			float wr, wi;
			Lobe(p - peakPos[i], wr, wi);
			const float d = wr * wr + wi * wi;
			ampRe[i] = (re * wr + im * wi) / d;
			ampIm[i] = (im * wr - re * wi) / d;
		}

	for (int i = 0; i < nPeaks; ++i)
	{
		const float pos = peakPos[i];
		int from = static_cast<int>(std::ceil(pos - K)), to = static_cast<int>(std::floor(pos + K));
		if (from < 1) from = 1;
		if (to > LB) to = LB;
		int idx = static_cast<int>(std::floor((from - pos) * kLobeSteps + 0.5f));
		for (int k = from; k <= to; ++k, idx += kLobeSteps)
		{
			const float wr = lobeRe[idx < 0 ? -idx : idx], wi = idx < 0 ? -lobeIm[-idx] : lobeIm[idx];
			X[k][0] -= ampRe[i] * wr - ampIm[i] * wi;
			X[k][1] -= ampRe[i] * wi + ampIm[i] * wr;
		}
		peakPhase[i] = std::atan2(ampIm[i], ampRe[i]);
	}
}

// One partial, skirt included, into the output spectrum at a fractional bin.
void OctaveEngine::AddLobe(float position, float re, float im)
{
	const int last = NB - 2;
	const int from = static_cast<int>(std::ceil(position - K));
	int to = static_cast<int>(std::floor(position + K));
	if (to > last) to = last;
	int idx = static_cast<int>(std::floor((from - position) * kLobeSteps + 0.5f));
	for (int k = from; k <= to; ++k, idx += kLobeSteps)
	{
		const float wr = lobeRe[idx < 0 ? -idx : idx], wi = idx < 0 ? -lobeIm[-idx] : lobeIm[idx];
		const float vr = re * wr - im * wi, vi = re * wi + im * wr;
		if (k >= 1)                       { Y[k][0] += vr;  Y[k][1] += vi; }
		else if (k <= -1 && -k <= last)   { Y[-k][0] += vr; Y[-k][1] -= vi; } // under DC: mirrored, conjugated
	}
}

void OctaveEngine::AddVoice(int voice, float gain, float detune)
{
	const float ratio = kRatio[voice];
	const int branches = kBranches[voice];
	const int last = NB - 2; // highest bin written; DC and Nyquist stay empty

	for (int i = 0; i < nPeaks; ++i)
	{
		const float pos = peakPos[i], phase = peakPhase[i];
		const float target = ratio * detune * pos;
		if (target > last - 2)
			break; // this partial and all above it would land over Nyquist

		const int lo = regionLo[i], hi = regionHi[i];

		// Detuning is the one thing that cannot be read off the input's phase: the extra frequency
		// is accumulated, per input bin, and added on top of the locked phase.
		float offset = 0;
		if (detune != 1.0f)
		{
			float* const memory = drift[voice];
			offset = wrapPhase(memory[peakBin[i]] + kTwoPi * (detune - 1.0f) * ratio * pos * H / N);
			for (int k = lo; k <= hi; ++k) memory[k] = offset;
		}

		float theta;
		if (branches == 1)
			theta = (ratio - 1.0f) * phase + offset;
		else
		{
			// phase / 2 (or / 4) is only known up to a half (quarter) turn: stay on the branch the
			// previous frame was on
			float* const memory = lastPhase[voice];
			const float predicted = memory[peakBin[i]] + kTwoPi * target * H / N;
			float best = 0, bestDistance = 1e9f;
			for (int m = 0; m < branches; ++m)
			{
				const float candidate = ratio * (phase + kTwoPi * m) + offset;
				const float distance = std::fabs(wrapPhase(candidate - predicted));
				if (distance < bestDistance) { bestDistance = distance; best = candidate; }
			}
			best = wrapPhase(best);
			for (int k = lo; k <= hi; ++k) memory[k] = best;
			theta = best - phase;
		}

		const float shift = target - pos;
		const float whole = std::floor(shift);
		const float frac = shift - whole;
		const int d0 = static_cast<int>(whole);
		const int fi = static_cast<int>(frac * kFracSteps + 0.5f);

		// gain, rotation and the interpolation's correction in one complex factor
		const float c = std::cos(theta), s = std::sin(theta);
		const float rr = gain * (c * fracRe[fi] - s * fracIm[fi]);
		const float ri = gain * (c * fracIm[fi] + s * fracRe[fi]);
		const float w0 = 1.0f - frac, w1 = frac;

		if (K > 0)
		{
			const float ar = gain * (ampRe[i] * c - ampIm[i] * s), ai = gain * (ampRe[i] * s + ampIm[i] * c);
			AddLobe(target, ar, ai);
		}

		if (lo + d0 >= 1 && hi + d0 + 1 <= last)
		{
			for (int j = lo; j <= hi; ++j)
			{
				const float re = X[j][0] * rr - X[j][1] * ri;
				const float im = X[j][0] * ri + X[j][1] * rr;
				const int d = j + d0;
				Y[d][0] += w0 * re;     Y[d][1] += w0 * im;
				Y[d + 1][0] += w1 * re; Y[d + 1][1] += w1 * im;
			}
		}
		else
		{
			// the edges: what falls under DC comes back mirrored and conjugated, what falls over
			// Nyquist is dropped
			for (int j = lo; j <= hi; ++j)
			{
				const float re = X[j][0] * rr - X[j][1] * ri;
				const float im = X[j][0] * ri + X[j][1] * rr;
				for (int t = 0; t < 2; ++t)
				{
					const int d = j + d0 + t;
					const float w = t == 0 ? w0 : w1;
					if (d >= 1 && d <= last)        { Y[d][0] += w * re;  Y[d][1] += w * im; }
					else if (d <= -1 && -d <= last) { Y[-d][0] += w * re; Y[-d][1] -= w * im; }
				}
			}
		}
	}
}

// A frame every two ticks, its work spread over both so that no block is heavier than the
// other: the tick that completes the frame's input analyses it, the next one builds the voices
// and starts sending them out. That costs one tick of latency and halves the peak load.
void OctaveEngine::Process(const float* in, float* wet, const float* gains, const float* detune)
{
	// the ring is circular: ringAt is the oldest sample
	for (int n = 0; n < T; ++n)
		ring[(ringAt + n) & (N - 1)] = in[n];
	ringAt = (ringAt + T) & (N - 1);

	float loudest = 0;
	for (int n = 0; n < T; ++n)
	{
		const float a = std::fabs(in[n]);
		if (a > loudest) loudest = a;
	}
	if (loudest < 1e-9f)
	{
		if (silentTicks < N / T) ++silentTicks;
	}
	else
		silentTicks = 0;

	if (!synthesisNext)
	{
		bool anyVoice = false;
		for (int v = 0; v < NVOICES; ++v)
			if (gains[v] > 0.0f) anyVoice = true;

		analysed = false;
		if (anyVoice && silentTicks < N / T && fwd && inv)
		{
			const int first = N - ringAt; // samples from ringAt to the end of the buffer
			for (int n = 0; n < first; ++n)
				timeIn[n] = ring[ringAt + n] * winA[n];
			for (int n = first; n < N; ++n)
				timeIn[n] = ring[n - first] * winA[n];
			fftwf_execute(fwd);

			FindPeaks();
			if (K > 0) ModelPartials();
			analysed = nPeaks > 0;
		}

		std::memcpy(wet, pending + T, sizeof(float) * T); // second half of the previous frame's output
	}
	else
	{
		if (analysed)
		{
			std::memset(Y, 0, sizeof(fftwf_complex) * NB);
			for (int v = 0; v < NVOICES; ++v)
				if (gains[v] > 0.0f) AddVoice(v, gains[v], detune ? detune[v] : 1.0f);

			for (int k = 0; k < NB; ++k)
			{
				const float re = Y[k][0] * refRe[k] + Y[k][1] * refIm[k];
				const float im = Y[k][1] * refRe[k] - Y[k][0] * refIm[k];
				Y[k][0] = re;
				Y[k][1] = im;
			}
			fftwf_execute(inv);

			const float* const tail = timeOut + (N - M);
			for (int m = 0; m < M; ++m)
				ola[m] += tail[m] * winS[m];
		}

		std::memcpy(pending, ola, sizeof(float) * H);
		std::memmove(ola, ola + H, sizeof(float) * (M - H));
		std::memset(ola + M - H, 0, sizeof(float) * H);

		std::memcpy(wet, pending, sizeof(float) * T);
	}
	synthesisNext = !synthesisNext;
}
