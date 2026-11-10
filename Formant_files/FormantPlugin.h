#ifndef MOD_PITCHSHIFTER_FORMANTPLUGIN_H
#define MOD_PITCHSHIFTER_FORMANTPLUGIN_H

/* LV2 wrapper shared by the formant-preserving Capo and Drop. Include after defining PLUGIN_URI.
 * Ports: In, Out, Step (semitones), Gain (dB), Window (frame size in samples), Formant (on/off). */

#include <stdlib.h>
#include <string.h>
#include <string>
#include <cmath>
#include "PitchShifterClasses.h"
#include "GainClass.h"
#include "BlockAdapter.h"
#include "FormantEngine.h"

#undef IN
#undef OUT

enum {IN, OUT, STEP, GAIN, WINDOW, FORMANT, PLUGIN_PORT_COUNT};

// window sizes offered (samples at a 128 hop; latency = window - 128). Default: 1536 (by ear, 2026-10-28).
static const int kWindows[] = {512, 768, 1024, 1536, 2048};
#define NWINDOWS 5
#define DEFAULT_WINDOW 3

// the guitar plugins follow a fixed f0 (chords: no pitch detector); by measurement 80-300 Hz all work
#define FORMANT_F0 200.0

// envelope method: 0 = true envelope (cepstral, 16 FFTs per hop), 1 = running max + smoothing (no FFT)
#ifndef FORMANT_ENV_MODE
#define FORMANT_ENV_MODE 1
#endif

// envelope update interval in hops (1 = every hop)
#ifndef FORMANT_ENV_EVERY
#define FORMANT_ENV_EVERY 1
#endif

class FormantPS
{
public:
	FormantPS(uint32_t n_samples, double samplerate, const std::string& wfile)
	{
		hop = n_samples;
		SampleRate = samplerate;

		// one engine per window size, built here: selecting one later is a pointer swap
		const bool haveWisdom = ImportWisdom(wfile.c_str());
		for (int i = 0; i < NWINDOWS; i++)
		{
			eng_all[i] = new FormantEngine(n_samples, nBuffersFor(i, n_samples), haveWisdom);
			eng_all[i]->envEvery = FORMANT_ENV_EVERY;
			eng_all[i]->envMode = FORMANT_ENV_MODE;
		}
		objg = new GainClass(n_samples);
		adapter = new BlockAdapter(n_samples, 1);

		window = -1;
		SelectWindow(DEFAULT_WINDOW);
	}
	~FormantPS()
	{
		for (int i = 0; i < NWINDOWS; i++)
			delete eng_all[i];
		delete objg;
		delete adapter;
	}

	static int nBuffersFor(int i, uint32_t n_samples)
	{
		int nb = (int)lround(kWindows[i] / (double)n_samples);
		return nb < 2 ? 2 : nb;
	}

	// the port carries the window size in samples; take the nearest one offered
	static int WindowIndex(float v)
	{
		int best = DEFAULT_WINDOW;
		float bd = 1e9f;
		for (int i = 0; i < NWINDOWS; i++)
		{
			const float d = fabsf(v - kWindows[i]);
			if (d < bd) { bd = d; best = i; }
		}
		return best;
	}

	void SelectWindow(int i)
	{
		if (i < 0 || i >= NWINDOWS || i == window)
			return;
		window = i;
		eng = eng_all[i];
		eng->Reset();
		cont = 0;
		adapter->Cancel();
	}

	bool Analyse(const float *in, float *out, double s, double gain, bool formant)
	{
		if (InputAbsSum(in, hop) == 0)
		{
			memset(out, 0, sizeof(float) * hop);
			return false;
		}

		objg->SetGaindB(gain);
		eng->Push(in);

		if (cont < eng->Q - 1)
		{
			cont = cont + 1;
			memset(out, 0, sizeof(float) * hop);
			return false;
		}

		step = s;
		form = formant;
		return true;
	}

	void Synthesise(float *out)
	{
		const double alpha = pow(2.0, step / 12.0);
		const int nc = (int)(SampleRate / (2.0 * FORMANT_F0 * (alpha > 1.0 ? alpha : 1.0)));
		eng->Process(step, nc, form);
		objg->SimpleGain(eng->yshift, out);
	}

	static LV2_Handle instantiate(const LV2_Descriptor*, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
	{
		std::string wisdomFile = bundle_path;
		wisdomFile += "/harmonizer.wisdom";
		const uint32_t n_samples = EngineHop(GetBufferSize(features));
		return (LV2_Handle)new FormantPS(n_samples, samplerate, wisdomFile);
	}
	static void activate(LV2_Handle) {}
	static void deactivate(LV2_Handle) {}
	static void connect_port(LV2_Handle instance, uint32_t port, void *data)
	{
		((FormantPS *)instance)->ports[port] = (float *)data;
	}
	static void run(LV2_Handle instance, uint32_t n_samples)
	{
		FormantPS *plugin = (FormantPS *)instance;

		const float *in   = plugin->ports[IN];
		float       *out  = plugin->ports[OUT];
		double       s    = (double)ControlInt(*plugin->ports[STEP]);
		double       gain = (double)(*plugin->ports[GAIN]);
		bool         fm   = *plugin->ports[FORMANT] > 0.5f;

		plugin->SelectWindow(WindowIndex(*plugin->ports[WINDOW]));

		float* const outs[1] = { out };
		plugin->adapter->Run(in, outs, n_samples,
			[&](const float *hin, float* const* houts) { return plugin->Analyse(hin, houts[0], s, gain, fm); },
			[&](float* const* houts) { plugin->Synthesise(houts[0]); });
	}
	static void cleanup(LV2_Handle instance)
	{
		delete (FormantPS *)instance;
	}
	static const void* extension_data(const char*) { return NULL; }

	float *ports[PLUGIN_PORT_COUNT];

private:
	FormantEngine *eng_all[NWINDOWS];
	FormantEngine *eng;
	GainClass *objg;
	BlockAdapter *adapter;
	double step;
	bool form;
	int window;
	int cont;
	uint32_t hop;
	double SampleRate;
};

static const LV2_Descriptor Descriptor = {
	PLUGIN_URI,
	FormantPS::instantiate,
	FormantPS::connect_port,
	FormantPS::activate,
	FormantPS::run,
	FormantPS::deactivate,
	FormantPS::cleanup,
	FormantPS::extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &Descriptor : NULL;
}

#endif
