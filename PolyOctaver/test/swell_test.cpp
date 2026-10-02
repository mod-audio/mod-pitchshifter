// The Attack control's note detector (src/Swell.h) against the cases that fool a level detector.

#include "Swell.h"
#include "measure.h"

static int failures = 0;

static void expectNotes(const std::vector<float>& x, const char* name, int want)
{
	Swell swell(kRate);
	std::vector<float> gain(128);
	for (size_t off = 0; off + 128 <= x.size(); off += 128)
		swell.Process(&x[off], gain.data(), 128, 300.0f);

	const bool ok = swell.Triggers() == want;
	std::printf("%-52s notes found %2d, want %2d%s\n", name, swell.Triggers(), want, ok ? "" : "  <-- FAIL");
	if (!ok) ++failures;
}

static std::vector<float> mixed(std::vector<float> a, const std::vector<float>& b)
{
	for (size_t i = 0; i < a.size() && i < b.size(); ++i) a[i] += b[i];
	return a;
}

int main()
{
	const double openE[6] = { 82.41, 123.47, 164.81, 207.65, 246.94, 329.63 };
	const double slowBeat[2] = { 220.0, 223.0 };
	const double roughBeat[2] = { 220.0, 250.0 };
	const size_t second = static_cast<size_t>(kRate);

	expectNotes(render(chord(openE, 6, 5, 0.05), 6 * second, 4800), "steady open E chord, 6 s", 1);
	expectNotes(render(chord(openE, 6, 5, 0.2), 6 * second, 4800, 1.5), "plucked open E chord, decaying, 6 s", 1);
	expectNotes(render(chord(slowBeat, 2, 3, 0.2), 6 * second, 4800), "two strings 3 Hz apart, beating", 1);
	expectNotes(render(chord(roughBeat, 2, 4, 0.2), 4 * second, 4800), "two notes 30 Hz apart, rough beating", 1);

	const std::vector<float> ringing = render(chord(openE, 6, 5, 0.2), 4 * second, 4800, 2.0);
	expectNotes(mixed(ringing, render(harmonicTone(440, 5, 0.15), 4 * second, 2 * second, 1.0)),
	            "chord, then a single note over it 2 s later", 2);
	expectNotes(mixed(ringing, render(harmonicTone(440, 5, 0.05), 4 * second, 2 * second, 1.0)),
	            "chord, then a soft single note over it", 2);

	// fast picking: 24 short notes, 8 a second
	std::vector<float> picking(4 * second, 0.0f);
	for (int n = 0; n < 24; ++n)
	{
		const std::vector<float> one = render(harmonicTone(196.0 * (1 + n % 3 * 0.125), 5, 0.3), 6000, 0, 0.15);
		for (size_t i = 0; i < one.size(); ++i) picking[second / 2 + n * 6000 + i] += one[i];
	}
	expectNotes(picking, "24 notes at 8 per second", 24);

	std::printf(failures ? "FAIL\n" : "ok\n");
	return failures ? 1 : 0;
}
