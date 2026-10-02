// OctaveEngine, one voice at a time: level of the expected partials, everything else, rise time, cost.
// Same signals and the same measurements as baseline_test.cpp.

#include "OctaveEngine.h"
#include "measure.h"

#include <chrono>
#include <cstdlib>
#include <cstring>

static std::vector<float> runVoice(const OctaveEngine::Config& cfg, const std::vector<float>& in, int voice, double* usPerHop = 0, double cents = 0)
{
	OctaveEngine engine(cfg, false);
	float gains[OctaveEngine::NVOICES] = { 0, 0, 0, 0, 0 };
	float detune[OctaveEngine::NVOICES];
	for (int v = 0; v < OctaveEngine::NVOICES; ++v) detune[v] = static_cast<float>(std::pow(2.0, cents / 1200.0));
	if (voice >= 0) gains[voice] = 1.0f;
	else for (int v = 0; v < OctaveEngine::NVOICES; ++v) gains[v] = 1.0f;

	std::vector<float> out(in.size(), 0.0f);
	const size_t hop = engine.Hop();
	const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
	size_t hops = 0;
	for (size_t off = 0; off + hop <= in.size(); off += hop, ++hops)
		engine.Process(&in[off], &out[off], gains, detune);
	if (usPerHop)
		*usPerHop = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - t0).count() / hops;
	return out;
}

int main(int argc, char** argv)
{
	const double triad[3] = { 220.0, 277.18, 329.63 };
	const double power[2] = { 82.41, 123.47 };
	const double openE[6] = { 82.41, 123.47, 164.81, 207.65, 246.94, 329.63 };
	struct { const char* name; Tone tone; double decay; } signals[] = {
		{ "A3 220 Hz, 6 harmonics", harmonicTone(220.0, 6, 0.2) , 0 },
		{ "E2 82.4 Hz, 8 harmonics", harmonicTone(82.41, 8, 0.2) , 0 },
		{ "A major triad, 4 harmonics", chord(triad, 3, 4, 0.1) , 0 },
		{ "E2+B2 power chord, 6 harmonics", chord(power, 2, 6, 0.1) , 0 },
		{ "open E major, six strings, 3 harmonics", chord(openE, 6, 3, 0.06) , 0 },
		{ "A5 880 Hz, 5 harmonics", harmonicTone(880.0, 5, 0.2) , 0 },
		{ "open E major, plucked, decaying 1 s", chord(openE, 6, 3, 0.3), 1.0 },
		{ "E2, plucked, decaying 0.5 s", harmonicTone(82.41, 8, 0.8), 0.5 },
	};
	// usage: engine_test [frame synth hop skirt band]...
	std::vector<OctaveEngine::Config> configs;
	for (int a = 1; a + 4 < argc; a += 5)
	{
		const OctaveEngine::Config c = { std::atoi(argv[a]), std::atoi(argv[a + 1]), std::atoi(argv[a + 2]), std::atoi(argv[a + 3]), std::atoi(argv[a + 4]) };
		configs.push_back(c);
	}
	if (configs.empty()) configs.push_back(OctaveEngine::DefaultConfig(kRate));
	const int nConfigs = static_cast<int>(configs.size());
	const char* voiceName[5] = { "-2", "-1", " 0", "+1", "+2" };
	const double ratio[5] = { 0.25, 0.5, 1.0, 2.0, 4.0 };

	const size_t total = 4 * 48000, from = 2 * 48000;

	for (int c = 0; c < nConfigs; ++c)
	{
		const OctaveEngine::Config& cfg = configs[c];
		std::printf("\n=== frame %d, synthesis %d, hop %d, skirt %d, band %d: latency %.1f ms ===\n", cfg.frame, cfg.synth, cfg.hop,
		            cfg.skirt, cfg.band, 1000.0 * (cfg.synth - cfg.hop / 2) / kRate);
		for (size_t sg = 0; sg < sizeof(signals) / sizeof(signals[0]); ++sg)
		{
			std::printf("%s\n", signals[sg].name);
			std::printf("  voice | gain dB mean (min..max) | spurious dB | input bleed dB | rise ms\n");
			const std::vector<float> in = render(signals[sg].tone, total, signals[sg].decay > 0 ? 48000 + 24000 : 0, signals[sg].decay);
			const std::vector<float> burst = render(signals[sg].tone, 48000, 12000);
			for (int v = 0; v < 5; ++v)
			{
				if (v == OctaveEngine::UNISON) continue; // in tune it is the input again; see the detuned runs
				const std::vector<float> out = runVoice(cfg, in, v);
				const VoiceResult r = measureVoice(&in[from], &out[from], signals[sg].tone, ratio[v]);
				const double rise = riseTimeMs(runVoice(cfg, burst, v), 12000);
				std::printf("  %s    | %+6.1f (%+6.1f..%+6.1f) | %+6.1f      | %+6.1f         | %5.1f\n",
				            voiceName[v], r.meanGainDb, r.minGainDb, r.maxGainDb, r.spuriousDb, r.inputBleedDb, rise);
			}
		}
		// detuned: the unison by 10 cents (what Dry FX does to the dry signal), the octave up by 30
		const std::vector<float> chordIn = render(signals[4].tone, total);
		const struct { int voice; double cents; } detuned[] = { { OctaveEngine::UNISON, 0 }, { OctaveEngine::UNISON, -10 }, { OctaveEngine::UP1, 30 }, { OctaveEngine::UP2, -30 }, { OctaveEngine::SUB1, 30 } };
		std::printf("open E major, detuned voices\n");
		for (size_t d = 0; d < sizeof(detuned) / sizeof(detuned[0]); ++d)
		{
			const std::vector<float> out = runVoice(cfg, chordIn, detuned[d].voice, 0, detuned[d].cents);
			const VoiceResult r = measureVoice(&chordIn[from], &out[from], signals[4].tone, ratio[detuned[d].voice] * std::pow(2.0, detuned[d].cents / 1200.0));
			std::printf("  %s %+3.0f c | %+6.1f (%+6.1f..%+6.1f) | %+6.1f      | %+6.1f\n", voiceName[detuned[d].voice], detuned[d].cents,
			            r.meanGainDb, r.minGainDb, r.maxGainDb, r.spuriousDb, r.inputBleedDb);
		}

		double us = 0, usNoise = 0;
		runVoice(cfg, render(signals[4].tone, total), -1, &us);
		std::vector<float> noise(total);
		unsigned seed = 12345;
		for (size_t i = 0; i < total; ++i)
		{
			seed = seed * 1664525u + 1013904223u;
			noise[i] = 0.2f * (static_cast<int>(seed >> 8) / 8388608.0f - 1.0f);
		}
		runVoice(cfg, noise, -1, &usNoise);
		std::printf("cost, all four voices, this host, per 128-sample tick: open E chord %.1f us, white noise %.1f us (%.2f %% of one core at 48 kHz)\n",
		            us, usNoise, 100.0 * usNoise / (1e6 * cfg.hop / 2 / kRate));
	}
	return 0;
}
