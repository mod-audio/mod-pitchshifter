#ifndef POLYOCTAVER_TEST_MEASURE_H
#define POLYOCTAVER_TEST_MEASURE_H

// Offline measurements for an octave voice: how loud the expected partials come out,
// and how much of the output is anything else.

#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <fftw3.h>

struct Partial { double freq, amp; };

typedef std::vector<Partial> Tone;

static const double kRate = 48000.0;

inline Tone harmonicTone(double f0, int harmonics, double amp)
{
	Tone t;
	for (int h = 1; h <= harmonics; ++h)
	{
		Partial p = { f0 * h, amp / h };
		t.push_back(p);
	}
	return t;
}

inline Tone chord(const double* f0s, int n, int harmonics, double amp)
{
	Tone t;
	for (int i = 0; i < n; ++i)
	{
		const Tone one = harmonicTone(f0s[i], harmonics, amp);
		t.insert(t.end(), one.begin(), one.end());
	}
	return t;
}

// Every partial starts at a different phase so the test does not depend on a lucky alignment.
// `decay` > 0: plucked at `onset`, every partial dying away with that time constant (seconds),
// the higher ones faster, as on a string.
inline std::vector<float> render(const Tone& tone, size_t samples, size_t onset = 0, double decay = 0)
{
	std::vector<float> x(samples, 0.0f);
	for (size_t k = 0; k < tone.size(); ++k)
	{
		const double w = 2.0 * M_PI * tone[k].freq / kRate, ph = 0.7 * k * k + 0.3;
		const double tau = decay > 0 ? decay * kRate / (1.0 + tone[k].freq / 1000.0) : 0;
		for (size_t n = onset; n < samples; ++n)
		{
			const double env = decay > 0 ? std::exp(-static_cast<double>(n - onset) / tau) : 1.0;
			x[n] += static_cast<float>(env * tone[k].amp * std::sin(w * n + ph));
		}
	}
	return x;
}

struct VoiceResult
{
	double meanGainDb, minGainDb, maxGainDb; // per expected partial, output level relative to the input partial
	double spuriousDb;                      // everything that is not an expected partial, relative to them
	double inputBleedDb;                    // input partials that should not be in the output, relative to expected
};

// Power spectrum of a Hann-windowed 65536-sample segment: 0.73 Hz per bin.
static const int kSpectrumSize = 65536;

inline std::vector<double> powerSpectrum(const float* x)
{
	const int n = kSpectrumSize;
	float* const t = fftwf_alloc_real(n);
	fftwf_complex* const f = fftwf_alloc_complex(n / 2 + 1);
	const fftwf_plan plan = fftwf_plan_dft_r2c_1d(n, t, f, FFTW_ESTIMATE);
	for (int i = 0; i < n; ++i)
		t[i] = static_cast<float>(x[i] * (0.5 - 0.5 * std::cos(2.0 * M_PI * i / n)));
	fftwf_execute(plan);
	std::vector<double> p(n / 2 + 1);
	for (int k = 0; k <= n / 2; ++k)
		p[k] = static_cast<double>(f[k][0]) * f[k][0] + static_cast<double>(f[k][1]) * f[k][1];
	fftwf_destroy_plan(plan);
	fftwf_free(f);
	fftwf_free(t);
	return p;
}

// Energy within +-4 Hz of each frequency. Partials closer than that to one another (3 x 220 Hz
// against 2 x 329.63 Hz in a triad) share a band: no analysis frame can tell them apart either.
struct Bands
{
	std::vector<double> centre, energy;
	std::vector<int> lo, hi;
	double mergeDistance;

	explicit Bands(double merge = 8.0) : mergeDistance(merge) {}

	void add(double freq)
	{
		if (freq < 15.0 || freq > 0.45 * kRate) return;
		for (size_t i = 0; i < centre.size(); ++i)
			if (std::fabs(centre[i] - freq) < mergeDistance)
			{
				// widen the existing band to cover both
				const int l = static_cast<int>(std::floor((freq - 4.0) * kSpectrumSize / kRate));
				const int h = static_cast<int>(std::ceil((freq + 4.0) * kSpectrumSize / kRate));
				if (l < lo[i]) lo[i] = l;
				if (h > hi[i]) hi[i] = h;
				return;
			}
		centre.push_back(freq);
		lo.push_back(static_cast<int>(std::floor((freq - 4.0) * kSpectrumSize / kRate)));
		hi.push_back(static_cast<int>(std::ceil((freq + 4.0) * kSpectrumSize / kRate)));
		energy.push_back(0);
	}

	void measure(const std::vector<double>& spectrum)
	{
		for (size_t i = 0; i < centre.size(); ++i)
		{
			energy[i] = 0;
			for (int k = lo[i] < 0 ? 0 : lo[i]; k <= hi[i] && k < static_cast<int>(spectrum.size()); ++k)
				energy[i] += spectrum[k];
		}
	}

	bool covers(int bin) const
	{
		for (size_t i = 0; i < centre.size(); ++i)
			if (bin >= lo[i] && bin <= hi[i]) return true;
		return false;
	}
};

// `in` and `out` are steady-state segments of kSpectrumSize samples; `out` is `tone` shifted by `ratio`.
inline VoiceResult measureVoice(const float* in, const float* out, const Tone& tone, double ratio)
{
	VoiceResult r = { 0, 1e9, -1e9, 0, 0 };

	Bands source(8.0 / ratio), expected, unwanted; // so both sides merge the same partials
	for (size_t k = 0; k < tone.size(); ++k)
		if (tone[k].freq * ratio >= 15.0 && tone[k].freq * ratio <= 0.45 * kRate)
		{
			source.add(tone[k].freq);
			expected.add(tone[k].freq * ratio);
		}
	const std::vector<double> inSpectrum = powerSpectrum(in), outSpectrum = powerSpectrum(out);
	source.measure(inSpectrum);
	expected.measure(outSpectrum);

	// bands pair up one to one as long as merging happened the same way on both sides
	double wanted = 0;
	const size_t n = expected.centre.size();
	for (size_t i = 0; i < n; ++i)
	{
		wanted += expected.energy[i];
		if (source.centre.size() != n) continue;
		const double gainDb = 10.0 * std::log10(expected.energy[i] / (source.energy[i] + 1e-30) + 1e-30);
		if (std::getenv("MEASURE_VERBOSE"))
			std::printf("      %8.2f Hz -> %8.2f Hz  %+6.1f dB\n", source.centre[i], expected.centre[i], gainDb);
		r.meanGainDb += gainDb / n;
		if (gainDb < r.minGainDb) r.minGainDb = gainDb;
		if (gainDb > r.maxGainDb) r.maxGainDb = gainDb;
	}

	double total = 0;
	for (size_t k = 0; k < outSpectrum.size(); ++k) total += outSpectrum[k];

	for (size_t k = 0; k < tone.size(); ++k)
	{
		const int bin = static_cast<int>(tone[k].freq * kSpectrumSize / kRate + 0.5);
		if (!expected.covers(bin)) unwanted.add(tone[k].freq);
	}
	unwanted.measure(outSpectrum);
	double bleed = 0;
	for (size_t i = 0; i < unwanted.energy.size(); ++i) bleed += unwanted.energy[i];

	r.spuriousDb = 10.0 * std::log10((total - wanted) / (wanted + 1e-30) + 1e-30);
	r.inputBleedDb = 10.0 * std::log10(bleed / (wanted + 1e-30) + 1e-30);
	return r;
}

// Samples from the onset until the output envelope first reaches half of its settled level.
inline double riseTimeMs(const std::vector<float>& out, size_t onset)
{
	const size_t win = 96; // 2 ms
	std::vector<double> env;
	for (size_t i = onset; i + win <= out.size(); i += win / 2)
	{
		double s = 0;
		for (size_t j = 0; j < win; ++j) s += out[i + j] * out[i + j];
		env.push_back(std::sqrt(s / win));
	}
	if (env.size() < 40) return -1;
	double settled = 0;
	for (size_t i = env.size() - 20; i < env.size(); ++i) settled += env[i];
	settled /= 20;
	for (size_t i = 0; i < env.size(); ++i)
		if (env[i] >= 0.5 * settled)
			return 1000.0 * (i * win / 2) / kRate;
	return -1;
}

#endif
