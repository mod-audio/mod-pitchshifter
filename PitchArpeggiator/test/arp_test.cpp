// Loads the built plugin like a host would (no lilv needed) and checks what an arpeggiator has to
// get right: the note of every step, when it changes against the beat, the gate, the freeze, host
// sync and odd block sizes.
// usage: arp_test <path to mod-pitcharpeggiator.so> <bundle dir/> [input.wav-less raw float32 mono 48k file, output file]

#include <dlfcn.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#include "measure.h"
#include "Sequence.h"

enum { IN, OUT, EVENTS, SYNC, BPM, DIVISION, PATTERN, ARP_MODE, OCTAVE, OCTAVE_SPREAD, OCTAVE_MODE, NOTE_LENGTH, FREEZE, DRY, LEVEL, NPORTS };

// a URID map good enough for one plugin
static std::vector<std::string> uris;
static LV2_URID mapUri(LV2_URID_Map_Handle, const char* uri)
{
	for (size_t i = 0; i < uris.size(); ++i)
		if (uris[i] == uri) return static_cast<LV2_URID>(i + 1);
	uris.push_back(uri);
	return static_cast<LV2_URID>(uris.size());
}

struct Controls
{
	float v[NPORTS];
	Controls()
	{
		std::memset(v, 0, sizeof(v));
		v[SYNC] = 0; v[BPM] = 120; v[DIVISION] = 6; v[PATTERN] = 0; v[ARP_MODE] = 0; v[OCTAVE] = 0;
		v[OCTAVE_SPREAD] = 1; v[OCTAVE_MODE] = 4; v[NOTE_LENGTH] = 100; v[FREEZE] = 0; v[DRY] = 0; v[LEVEL] = 100;
	}
};

struct HostTime { bool send; double bpm, bar, barBeat; size_t atSample; };

static const LV2_Descriptor* descriptor = 0;
static const char* bundle = "";

static std::vector<float> runPlugin(const std::vector<float>& in, Controls c, uint32_t block = 128, long freezeAt = -1,
                                    const HostTime* host = 0)
{
	LV2_URID_Map map = { 0, mapUri };
	const LV2_Feature mapFeature = { LV2_URID__map, &map };
	const LV2_Feature* const features[] = { &mapFeature, 0 };
	const LV2_Handle h = descriptor->instantiate(descriptor, kRate, bundle, features);

	union { LV2_Atom_Sequence seq; uint8_t bytes[1024]; } events;
	LV2_Atom_Forge forge;
	lv2_atom_forge_init(&forge, &map);
	const LV2_URID position = mapUri(0, LV2_TIME__Position), bar = mapUri(0, LV2_TIME__bar), barBeat = mapUri(0, LV2_TIME__barBeat),
	               bpb = mapUri(0, LV2_TIME__beatsPerBar), bpm = mapUri(0, LV2_TIME__beatsPerMinute), speed = mapUri(0, LV2_TIME__speed);

	std::vector<float> out(in.size(), 0.0f), inCopy(in);
	for (uint32_t p = SYNC; p < NPORTS; ++p) descriptor->connect_port(h, p, &c.v[p]);
	descriptor->connect_port(h, EVENTS, &events);
	if (descriptor->activate) descriptor->activate(h);
	for (size_t off = 0; off + block <= in.size(); off += block)
	{
		if (freezeAt >= 0 && static_cast<long>(off) >= freezeAt) c.v[FREEZE] = 1;

		lv2_atom_forge_set_buffer(&forge, events.bytes, sizeof(events.bytes));
		LV2_Atom_Forge_Frame seqFrame;
		lv2_atom_forge_sequence_head(&forge, &seqFrame, 0);
		if (host && host->send && host->atSample >= off && host->atSample < off + block)
		{
			LV2_Atom_Forge_Frame frame;
			lv2_atom_forge_frame_time(&forge, static_cast<int64_t>(host->atSample - off));
			lv2_atom_forge_object(&forge, &frame, 0, position);
			lv2_atom_forge_key(&forge, bar);     lv2_atom_forge_long(&forge, static_cast<int64_t>(host->bar));
			lv2_atom_forge_key(&forge, barBeat); lv2_atom_forge_float(&forge, static_cast<float>(host->barBeat));
			lv2_atom_forge_key(&forge, bpb);     lv2_atom_forge_float(&forge, 4.0f);
			lv2_atom_forge_key(&forge, bpm);     lv2_atom_forge_float(&forge, static_cast<float>(host->bpm));
			lv2_atom_forge_key(&forge, speed);   lv2_atom_forge_float(&forge, 1.0f);
			lv2_atom_forge_pop(&forge, &frame);
		}
		lv2_atom_forge_pop(&forge, &seqFrame);

		descriptor->connect_port(h, IN, &inCopy[off]);
		descriptor->connect_port(h, OUT, &out[off]);
		descriptor->run(h, block);
	}
	if (descriptor->deactivate) descriptor->deactivate(h);
	descriptor->cleanup(h);
	return out;
}

// Level of one frequency in a Hann-windowed stretch.
static double levelAt(const std::vector<float>& x, size_t from, size_t n, double freq)
{
	double re = 0, im = 0;
	for (size_t i = 0; i < n && from + i < x.size(); ++i)
	{
		const double w = 0.5 - 0.5 * std::cos(2.0 * M_PI * i / n);
		re += w * x[from + i] * std::cos(2.0 * M_PI * freq * i / kRate);
		im += w * x[from + i] * std::sin(2.0 * M_PI * freq * i / kRate);
	}
	return 4.0 * std::sqrt(re * re + im * im) / n; // amplitude of a sine at that frequency
}

static double rms(const std::vector<float>& x, size_t from, size_t n)
{
	double s = 0;
	for (size_t i = 0; i < n && from + i < x.size(); ++i) s += static_cast<double>(x[from + i]) * x[from + i];
	return std::sqrt(s / n);
}

static double db(double x) { return 20.0 * std::log10(x + 1e-12); }

// Which of the pattern's intervals is the strongest in a 20 ms window centred on `centre`.
static int noteAt(const std::vector<float>& out, size_t centre, double f0, double* level = 0)
{
	int best = 0; double bestLevel = -1;
	for (int s = -24; s <= 24; ++s)
	{
		const double l = levelAt(out, centre - 480, 960, f0 * std::pow(2.0, s / 12.0));
		if (l > bestLevel) { bestLevel = l; best = s; }
	}
	if (level) *level = bestLevel;
	return best;
}

static int failures = 0;
static void check(bool ok, const char* what)
{
	if (!ok) { ++failures; std::printf("  FAILED: %s\n", what); }
}

// The moment, in samples, around `boundary` at which the note `to` becomes stronger than `from`.
static double changeAt(const std::vector<float>& out, size_t boundary, double f0, int from, int to)
{
	const double fa = f0 * std::pow(2.0, from / 12.0), fb = f0 * std::pow(2.0, to / 12.0);
	for (long t = static_cast<long>(boundary) - 2400; t < static_cast<long>(boundary) + 2400; t += 12)
		if (levelAt(out, t - 240, 480, fb) > levelAt(out, t - 240, 480, fa)) return static_cast<double>(t);
	return -1e9;
}

int main(int argc, char** argv)
{
	if (argc < 3) { std::fprintf(stderr, "usage: %s <plugin.so> <bundle dir/> [in.f32 out.f32 pattern division]\n", argv[0]); return 2; }
	void* const lib = dlopen(argv[1], RTLD_NOW);
	if (!lib) { std::fprintf(stderr, "%s\n", dlerror()); return 1; }
	typedef const LV2_Descriptor* (*DescriptorFn)(uint32_t);
	const DescriptorFn fn = (DescriptorFn)dlsym(lib, "lv2_descriptor");
	descriptor = fn ? fn(0) : 0;
	bundle = argv[2];
	if (!descriptor) { std::fprintf(stderr, "no descriptor\n"); return 1; }
	std::printf("%s\n", descriptor->URI);

	// a file through the plugin, for listening: raw float32 mono 48 kHz in and out
	if (argc >= 5)
	{
		std::vector<float> in;
		if (FILE* f = std::fopen(argv[3], "rb"))
		{
			float buf[4096]; size_t n;
			while ((n = std::fread(buf, sizeof(float), 4096, f)) > 0) in.insert(in.end(), buf, buf + n);
			std::fclose(f);
		}
		Controls c;
		// arguments from the sixth on: port index = value
		for (int a = 5; a < argc; ++a)
		{
			int port; float value;
			if (std::sscanf(argv[a], "%d=%f", &port, &value) == 2 && port >= SYNC && port < NPORTS) c.v[port] = value;
		}
		long freezeAt = -1;
		if (c.v[FREEZE] > 1.5f) { freezeAt = static_cast<long>(c.v[FREEZE]); c.v[FREEZE] = 0; } // a sample number
		const std::vector<float> out = runPlugin(in, c, 128, freezeAt);
		if (FILE* f = std::fopen(argv[4], "wb")) { std::fwrite(&out[0], sizeof(float), out.size(), f); std::fclose(f); }
		return 0;
	}

	// 1. the sequences
	std::printf("\nsequences (Major, 16 steps)\n");
	const char* const arpNames[] = { "Up", "Down", "Up-Down", "Up-Down (alt)", "Random" };
	const char* const octNames[] = { "Up", "Down", "Up-Down", "Down-Up", "Cycle" };
	for (int oct = 4; oct >= 0; oct -= 4)
		for (int arp = 0; arp < 5; ++arp)
		{
			std::printf("  %-13s, 2 octaves, octave mode %-5s:", arpNames[arp], octNames[oct]);
			for (long n = 0; n < 16; ++n) std::printf(" %+d", Sequence::Semitones(n, 0, arp, 0, 2, oct));
			std::printf("\n");
		}
	{
		const int want[6] = { 0, 4, 7, 12, 16, 19 };
		bool ok = true;
		for (long n = 0; n < 12; ++n) if (Sequence::Semitones(n, 0, 0, 0, 2, 4) != want[n % 6]) ok = false;
		check(ok, "Major, Up, 2 octaves is not 0 4 7 12 16 19");
		for (int pattern = 0; pattern < Sequence::kNumPatterns; ++pattern)
			for (int arp = 0; arp < 5; ++arp) for (int o = -2; o <= 1; ++o) for (int sp = 1; sp <= 3; ++sp) for (int om = 0; om < 5; ++om)
				for (long n = 0; n < 200; ++n)
				{
					const int s = Sequence::Semitones(n, pattern, arp, o, sp, om);
					if (s < -24 || s > 24) ok = false;
				}
		check(ok, "an interval outside two octaves");
	}

	const double f0 = 220.0;
	const size_t step = 12000; // 120 BPM, 1/8
	const std::vector<float> sine = render(harmonicTone(f0, 1, 0.3), 6 * 48000);

	// 2. notes and timing, legato
	std::printf("\nA3 sine, 120 BPM, 1/8, Major, Up, 2 octaves, Note Length 100 %%\n  step | wanted | heard | level dB | change to it, ms from the beat\n");
	{
		Controls c; c.v[OCTAVE_SPREAD] = 2;
		const std::vector<float> out = runPlugin(sine, c);
		double worst = 0;
		for (size_t n = 2; n < 20; ++n)
		{
			const int wanted = Sequence::Semitones(n, 0, 0, 0, 2, 4), before = Sequence::Semitones(n - 1, 0, 0, 0, 2, 4);
			double level;
			const int heard = noteAt(out, n * step + step / 2, f0, &level);
			const double at = (changeAt(out, n * step, f0, before, wanted) - n * step) * 1000.0 / kRate;
			std::printf("  %4zu | %+6d | %+5d | %+8.1f | %+5.1f\n", n, wanted, heard, db(level / 0.3), at);
			check(heard == wanted, "wrong note");
			check(std::fabs(db(level / 0.3)) < 1.0, "note more than 1 dB off the input's level");
			if (std::fabs(at) > worst) worst = std::fabs(at);
		}
		check(worst < 4.0, "a note changes more than 4 ms from the beat");
	}

	// 3. the gate
	std::printf("\nsame, Note Length 50 %%: level through one step (dB re input), every 1/16 of the step\n ");
	{
		Controls c; c.v[NOTE_LENGTH] = 50;
		const std::vector<float> out = runPlugin(sine, c);
		const double ref = 0.3 / std::sqrt(2.0);
		for (int i = 0; i < 16; ++i)
			std::printf(" %+.0f", db(rms(out, 8 * step + i * step / 16, step / 16) / ref));
		std::printf("\n");
		check(std::fabs(db(rms(out, 8 * step + step / 8, step / 4) / ref)) < 1.0, "open gate is not at the input's level");
		check(db(rms(out, 8 * step + 5 * step / 8, step / 4) / ref) < -60.0, "closed gate lets sound through");
		// the half-way point of the gate opening against the beat
		size_t t = 8 * step - 480;
		while (t < 8 * step + 2400 && rms(out, t, 48) < 0.5 * ref) t += 12;
		std::printf("  gate half open %+.1f ms from the beat; note at that point %+d (wanted %+d)\n",
		            (static_cast<double>(t) - 8 * step) * 1000.0 / kRate, noteAt(out, t + 600, f0), Sequence::Semitones(8, 0, 0, 0, 1, 4));
		check(noteAt(out, t + 600, f0) == Sequence::Semitones(8, 0, 0, 0, 1, 4), "the gate opens on the wrong note");
	}

	// 4. freeze
	std::printf("\nfreeze: A major triad plucked at 0.1 s and decaying, frozen at 0.5 s, input cut at 1 s; Octaves pattern (no shift)\n");
	{
		const double triad[3] = { 220.0, 277.18, 329.63 };
		const Tone tone = chord(triad, 3, 4, 0.1);
		std::vector<float> in = render(tone, 8 * 48000, 4800, 1.0);
		for (size_t i = 48000; i < in.size(); ++i) in[i] = 0.0f;
		Controls c; c.v[PATTERN] = 8;
		const std::vector<float> out = runPlugin(in, c, 128, 24000);
		const double atFreeze = rms(in, 24000 - 2048, 4096);
		std::printf("  input level at the freeze %.1f dBFS; output per second:", db(atFreeze));
		for (int s = 1; s < 8; ++s) std::printf(" %.1f", db(rms(out, s * 48000, 48000)));
		std::printf(" dBFS\n");
		check(std::fabs(db(rms(out, 7 * 48000, 48000)) - db(rms(out, 2 * 48000, 48000))) < 0.5, "the frozen sound does not hold its level");
		check(std::fabs(db(rms(out, 2 * 48000, 48000)) - db(atFreeze)) < 3.0, "the frozen sound is not at the level the input had");
		const VoiceResult r = measureVoice(&out[5 * 48000], &out[5 * 48000], tone, 1.0);
		std::printf("  frozen sound, 5 s on: everything that is not a partial of the chord %.1f dB under the chord\n", r.spuriousDb);
		check(r.spuriousDb < -25.0, "the frozen sound is not the chord");

		// released: silence again
		Controls c2; c2.v[PATTERN] = 8;
		const std::vector<float> noFreeze = runPlugin(in, c2);
		check(rms(noFreeze, 3 * 48000, 48000) < 1e-6, "sound without freeze after the input stopped");

		// frozen and arpeggiated: the notes of the steps, on the frozen fundamental
		Controls c3;
		const std::vector<float> arp = runPlugin(render(harmonicTone(f0, 1, 0.3), 8 * 48000), c3, 128, 24000);
		bool ok = true;
		for (size_t n = 10; n < 30; ++n)
			if (noteAt(arp, n * step + step / 2, f0) != Sequence::Semitones(n, 0, 0, 0, 1, 4)) ok = false;
		check(ok, "frozen sound does not follow the arpeggio");
	}

	// 5. host sync
	std::printf("\nhost sync: 90 BPM, position bar 2 beat 1.5 sent at sample 10000, 1/8\n");
	{
		Controls c; c.v[SYNC] = 1;
		const HostTime host = { true, 90.0, 2.0, 1.5, 10000 };
		const std::vector<float> out = runPlugin(sine, c, 128, -1, &host);
		// beat 9.5 at sample 10000; a step is half a beat = 16000 samples; step 19 starts there
		const double stepSamples = 0.5 * 60.0 / 90.0 * kRate;
		double worst = 0; bool ok = true;
		for (int k = 1; k < 12; ++k)
		{
			const long n = 19 + k;
			const size_t boundary = static_cast<size_t>(10000 + k * stepSamples);
			const int wanted = Sequence::Semitones(n, 0, 0, 0, 1, 4), before = Sequence::Semitones(n - 1, 0, 0, 0, 1, 4);
			if (noteAt(out, boundary + static_cast<size_t>(stepSamples / 2), f0) != wanted) ok = false;
			const double at = (changeAt(out, boundary, f0, before, wanted) - boundary) * 1000.0 / kRate;
			if (std::fabs(at) > worst) worst = std::fabs(at);
		}
		std::printf("  notes %s, changes within %.1f ms of the host's beat\n", ok ? "as the song position says" : "WRONG", worst);
		check(ok, "host sync: wrong notes");
		check(worst < 4.0, "host sync: a note changes more than 4 ms from the beat");
	}

	// 6. block sizes
	std::printf("\nblock sizes\n");
	{
		const uint32_t blocks[] = { 128, 256, 64, 100, 1024 };
		std::vector<std::vector<float> > outs;
		for (size_t b = 0; b < 5; ++b)
		{
			Controls c; c.v[NOTE_LENGTH] = 70; c.v[DRY] = 50;
			outs.push_back(runPlugin(sine, c, blocks[b]));
			double peak = 0; bool finite = true;
			for (size_t i = 0; i < outs[b].size(); ++i) { if (!std::isfinite(outs[b][i])) finite = false; if (std::fabs(outs[b][i]) > peak) peak = std::fabs(outs[b][i]); }
			std::printf("  block %4u: peak %.3f, %s\n", blocks[b], peak, finite ? "finite" : "NOT FINITE");
			check(finite && peak > 0.05 && peak < 2.0, "bad output");
		}
		double diff = 0;
		for (size_t i = 0; i < 4 * 48000; ++i) diff = std::fmax(diff, std::fabs(outs[1][i] - outs[0][i]));
		std::printf("  256 against 128: largest difference %.2g\n", diff);
		check(diff < 1e-6, "256-sample blocks do not match 128");
	}

	std::printf("\n%s\n", failures ? "FAILURES" : "all checks passed");
	return failures ? 1 : 0;
}
