// Loads the built plugin like a host would (no lilv needed) and runs it at several block sizes:
// output must be finite, not silent, and the same whatever the block size once latency is allowed for.
// usage: plugin_smoke <path to mod-polyoctaver.so> <bundle dir>

#include <dlfcn.h>
#include <cmath>
#include <cstdio>
#include <vector>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#include "measure.h"

int main(int argc, char** argv)
{
	if (argc < 3) { std::fprintf(stderr, "usage: %s <plugin.so> <bundle dir/>\n", argv[0]); return 2; }
	void* const lib = dlopen(argv[1], RTLD_NOW);
	if (!lib) { std::fprintf(stderr, "%s\n", dlerror()); return 1; }
	typedef const LV2_Descriptor* (*DescriptorFn)(uint32_t);
	const DescriptorFn fn = (DescriptorFn)dlsym(lib, "lv2_descriptor");
	const LV2_Descriptor* const d = fn ? fn(0) : 0;
	if (!d) { std::fprintf(stderr, "no descriptor\n"); return 1; }
	std::printf("%s\n", d->URI);

	const double openE[6] = { 82.41, 123.47, 164.81, 207.65, 246.94, 329.63 };
	const std::vector<float> in = render(chord(openE, 6, 3, 0.06), 96000, 4800);
	const LV2_Feature* const features[] = { 0 };
	const uint32_t blocks[] = { 128, 256, 100, 1024 };
	std::vector<std::vector<float> > outs;
	int rc = 0;

	for (size_t b = 0; b < 4; ++b)
	{
		const LV2_Handle h = d->instantiate(d, 48000.0, argv[2], features);
		if (!h) { std::fprintf(stderr, "instantiate failed\n"); return 1; }
		//                Dry Sub2 Sub1 Up1 Up2 Attack Filter Reson Detune DryFX Tracking
		float ctl[11] = { 0,  50,  70,  70, 50, 0,     8000,  2,    10,    0,    1 };
		std::vector<float> out(in.size(), 0.0f), inCopy(in);
		for (uint32_t p = 0; p < 11; ++p) d->connect_port(h, 2 + p, &ctl[p]);
		if (d->activate) d->activate(h);
		for (size_t off = 0; off + blocks[b] <= in.size(); off += blocks[b])
		{
			if (off >= 48000) ctl[10] = 2; // switch Tracking while running
			d->connect_port(h, 0, &inCopy[off]);
			d->connect_port(h, 1, &out[off]);
			d->run(h, blocks[b]);
		}
		if (d->deactivate) d->deactivate(h);
		d->cleanup(h);

		double peak = 0; bool finite = true;
		for (size_t i = 0; i < out.size(); ++i) { if (!std::isfinite(out[i])) finite = false; if (std::fabs(out[i]) > peak) peak = std::fabs(out[i]); }
		std::printf("block %4u: peak %.3f, %s\n", blocks[b], peak, finite ? "finite" : "NOT FINITE");
		if (!finite || peak < 0.01 || peak > 2.0) rc = 1;
		outs.push_back(out);
	}

	// 256 is two hops a block and must match 128 exactly; 100 and 1024-with-remainder go through the adapter, one hop late
	double diff256 = 0, diff100 = 0;
	for (size_t i = 0; i < 40000; ++i) // the Tracking switch lands on a different sample per block size: compare before it
	{
		diff256 = std::fmax(diff256, std::fabs(outs[1][i] - outs[0][i]));
		diff100 = std::fmax(diff100, std::fabs(outs[2][i + 128] - outs[0][i]));
	}
	std::printf("256 vs 128, before the Tracking switch: max difference %.2e (must be 0)\n", diff256);
	std::printf("100 vs 128, shifted one hop, before the Tracking switch: max difference %.2e (must be 0)\n", diff100);
	if (diff256 != 0 || diff100 != 0) rc = 1;
	// Attack: three plucks a second apart, Dry FX on so that the dry signal swells too. 20 ms into
	// each note the output must still be far below where it is 600 ms in.
	{
		const size_t second = 48000;
		std::vector<float> plucks(4 * second, 0.0f);
		for (int note = 0; note < 3; ++note)
		{
			const std::vector<float> one = render(harmonicTone(note == 1 ? 196.0 : 146.83, 6, 0.3), second, 0, 0.8);
			for (size_t i = 0; i < second; ++i) plucks[second / 2 + note * second + i] += one[i];
		}
		for (int mode = 0; mode < 3; ++mode) // dry only / dry detuned through the engine / voices only
		{
			const LV2_Handle h = d->instantiate(d, 48000.0, argv[2], features);
			float ctl[11] = { 100, 0, 0, 0, 0, 500, 20000, 0.7f, 0, 1, 1 };
			if (mode == 1) ctl[8] = 15;
			if (mode == 2) { ctl[0] = 0; ctl[3] = 100; ctl[9] = 0; }
			std::vector<float> out(plucks.size(), 0.0f);
			for (uint32_t p = 0; p < 11; ++p) d->connect_port(h, 2 + p, &ctl[p]);
			if (d->activate) d->activate(h);
			for (size_t off = 0; off + 128 <= plucks.size(); off += 128)
			{
				d->connect_port(h, 0, &plucks[off]);
				d->connect_port(h, 1, &out[off]);
				d->run(h, 128);
			}
			d->cleanup(h);
			for (int note = 0; note < 3; ++note)
			{
				const size_t at = second / 2 + note * second;
				double early = 0, late = 0, inLate = 0;
				for (size_t i = 0; i < 960; ++i)
				{
					early = std::fmax(early, std::fabs(out[at + 480 + i]));          // 10..30 ms
					late = std::fmax(late, std::fabs(out[at + 28800 + i]));          // 600..620 ms
					inLate = std::fmax(inLate, std::fabs(plucks[at + 28800 + i]));
				}
				std::printf("attack 500 ms, %s, note %d: 10-30 ms in %.4f, 600 ms in %.4f (input there %.4f)\n",
				            mode == 0 ? "dry through Dry FX" : mode == 1 ? "dry detuned" : "Up 1 only", note + 1, early, late, inLate);
				if (early > 0.1 * late || late < 0.5 * inLate) rc = 1;
			}
		}
	}

	std::printf(rc ? "FAIL\n" : "ok\n");
	return rc;
}
