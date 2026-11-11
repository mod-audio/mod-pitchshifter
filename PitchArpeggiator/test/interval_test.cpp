// The Poly Octaver engine at intervals that are not octaves: one voice, an octave voice detuned
// to the interval. Level of the expected partials and everything else, per interval, for the
// unison voice and for the octave voice nearest to the interval.

#include "OctaveEngine.h"
#include "measure.h"

static std::vector<float> runVoice(const std::vector<float>& in, int voice, double detune)
{
	OctaveEngine engine(OctaveEngine::DefaultConfig(kRate), false);
	float gains[OctaveEngine::NVOICES] = { 0, 0, 0, 0, 0 };
	float det[OctaveEngine::NVOICES] = { 1, 1, 1, 1, 1 };
	gains[voice] = 1.0f;
	det[voice] = static_cast<float>(detune);
	std::vector<float> out(in.size(), 0.0f);
	const size_t hop = engine.Hop();
	for (size_t off = 0; off + hop <= in.size(); off += hop)
		engine.Process(&in[off], &out[off], gains, det);
	return out;
}

int main()
{
	const double triad[3] = { 220.0, 277.18, 329.63 };
	const double openE[6] = { 82.41, 123.47, 164.81, 207.65, 246.94, 329.63 };
	struct { const char* name; Tone tone; } signals[] = {
		{ "A3 220 Hz, 6 harmonics", harmonicTone(220.0, 6, 0.2) },
		{ "E2 82.4 Hz, 8 harmonics", harmonicTone(82.41, 8, 0.2) },
		{ "A major triad, 4 harmonics", chord(triad, 3, 4, 0.1) },
		{ "open E major, six strings, 3 harmonics", chord(openE, 6, 3, 0.06) },
	};
	const int semis[] = { -24, -12, -5, 0, 3, 4, 5, 7, 10, 12, 16, 19, 24 };
	const double octave[5] = { 0.25, 0.5, 1.0, 2.0, 4.0 };
	const size_t total = 4 * 48000, from = 2 * 48000;

	for (size_t sg = 0; sg < sizeof(signals) / sizeof(signals[0]); ++sg)
	{
		std::printf("%s\n  semitones | unison voice: gain dB (min..max), spurious dB | nearest octave voice: same\n", signals[sg].name);
		const std::vector<float> in = render(signals[sg].tone, total);
		for (size_t s = 0; s < sizeof(semis) / sizeof(semis[0]); ++s)
		{
			const double ratio = std::pow(2.0, semis[s] / 12.0);
			int nearest = 2;
			for (int v = 0; v < 5; ++v)
				if (std::fabs(std::log(ratio / octave[v])) < std::fabs(std::log(ratio / octave[nearest]))) nearest = v;
			const std::vector<float> a = runVoice(in, OctaveEngine::UNISON, ratio);
			const std::vector<float> b = runVoice(in, nearest, ratio / octave[nearest]);
			const VoiceResult ra = measureVoice(&in[from], &a[from], signals[sg].tone, ratio);
			const VoiceResult rb = measureVoice(&in[from], &b[from], signals[sg].tone, ratio);
			std::printf("  %+3d       | %+5.1f (%+5.1f..%+5.1f) %+6.1f | %+5.1f (%+5.1f..%+5.1f) %+6.1f\n", semis[s],
			            ra.meanGainDb, ra.minGainDb, ra.maxGainDb, ra.spuriousDb, rb.meanGainDb, rb.minGainDb, rb.maxGainDb, rb.spuriousDb);
		}
	}
	return 0;
}
