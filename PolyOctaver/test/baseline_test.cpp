// What the family's existing engine (PSAnalysis + PSSinthesis, as used by 2Voices/SuperCapo)
// delivers on the four octave shifts a poly octaver needs. Reference numbers for OctaveEngine.

#include "PitchShifterClasses.h"
#include "measure.h"

static std::vector<float> runOld(const std::vector<float>& in, int nBuffers, double semitones)
{
	const uint32_t hop = 128;
	PSAnalysis a(hop, nBuffers, false);
	PSSinthesis s(&a, false);
	std::vector<float> out(in.size(), 0.0f);
	int cont = 0;
	for (size_t off = 0; off + hop <= in.size(); off += hop)
	{
		a.PreAnalysis(nBuffers, &in[off]);
		s.PreSinthesis();
		if (cont < nBuffers - 1) { ++cont; continue; }
		a.Analysis();
		s.Sinthesis(semitones);
		for (uint32_t i = 0; i < hop; ++i) out[off + i] = static_cast<float>(s.yshift[i]);
	}
	return out;
}

int main()
{
	const double triad[3] = { 220.0, 277.18, 329.63 };
	struct { const char* name; Tone tone; } signals[] = {
		{ "A3 220 Hz, 6 harmonics", harmonicTone(220.0, 6, 0.2) },
		{ "E2 82.4 Hz, 8 harmonics", harmonicTone(82.41, 8, 0.2) },
		{ "A major triad, 4 harmonics", chord(triad, 3, 4, 0.1) },
	};
	const int fidelityBuffers[4] = { 6, 12, 16, 20 }; // 128-frame column of FIDELITY0..3
	const double steps[4] = { -24, -12, 12, 24 };

	const size_t total = 4 * 48000, from = 2 * 48000;

	for (size_t sg = 0; sg < 3; ++sg)
	{
		std::printf("\n%s\n", signals[sg].name);
		std::printf("  fidelity   N    step | gain dB mean (min..max) | spurious dB | input bleed dB\n");
		const std::vector<float> in = render(signals[sg].tone, total);
		for (int f = 0; f < 4; ++f)
			for (int st = 0; st < 4; ++st)
			{
				const std::vector<float> out = runOld(in, fidelityBuffers[f], steps[st]);
				const VoiceResult r = measureVoice(&in[from], &out[from], signals[sg].tone, std::pow(2.0, steps[st] / 12.0));
				std::printf("  %d       %5d  %+4.0f  | %+6.1f (%+6.1f..%+6.1f) | %+6.1f      | %+6.1f\n",
				            f, fidelityBuffers[f] * 128, steps[st], r.meanGainDb, r.minGainDb, r.maxGainDb,
				            r.spuriousDb, r.inputBleedDb);
			}
	}
	return 0;
}
