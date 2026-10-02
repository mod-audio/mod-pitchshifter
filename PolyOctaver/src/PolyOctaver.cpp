#include <cmath>
#include <cstring>
#include <string>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

#include "OctaveEngine.h"
#include "Swell.h"
#include "BlockAdapter.h"
#include "Wisdom.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/PolyOctaver"

enum {IN, OUT, DRY, SUB_2, SUB_1, UP_1, UP_2, ATTACK, FILTER, RESONANCE, DETUNE, DRY_FX, TRACKING, PLUGIN_PORT_COUNT};

struct Settings
{
    float dry;                                // linear
    float voices[4];                          // Sub 2, Sub 1, Up 1, Up 2, linear
    float attack, cutoff, resonance, detune;  // ms, Hz, Q, cents
    bool dryFx;
};

// Synthesis window per Tracking setting, at 44.1/48 kHz: 8, 13 and 19 ms from input to voices.
#define NTRACKING 3
static const int kSynthLength[NTRACKING] = { 512, 768, 1024 };

/**********************************************************************************************************************************************************/

class PolyOctaver
{
public:
    PolyOctaver(double samplerate, const std::string& wfile)
    {
        SampleRate = samplerate;

        // One engine per Tracking setting, all built here: selecting one later is a pointer swap
        // and a history reset, safe on the audio thread.
        const bool haveWisdom = ImportWisdom(wfile.c_str());
        OctaveEngine::Config config = OctaveEngine::DefaultConfig(samplerate);
        const int scale = config.frame / 4096;
        for (int t = 0; t < NTRACKING; t++)
        {
            config.synth = kSynthLength[t] * scale;
            engines[t] = new OctaveEngine(config, haveWisdom);
        }
        hop = engines[0]->Hop();
        wet = new float[hop]();
        swellGain = new float[hop]();
        swell = new Swell(samplerate);
        adapter = new BlockAdapter(hop, 1);

        tracking = -1;
        SelectTracking(1);

        dryPost_1 = dryPre_1 = 0;
        cutoff_1 = 20000.0;
        ic1 = ic2 = 0;
    }
    ~PolyOctaver()
    {
        for (int t = 0; t < NTRACKING; t++)
            delete engines[t];
        delete[] wet;
        delete[] swellGain;
        delete swell;
        delete adapter;
    }

    // Out-of-range values keep the current engine.
    void SelectTracking(int t)
    {
        if (t < 0 || t >= NTRACKING || t == tracking)
            return;

        tracking = t;
        engine = engines[t];
        engine->Reset();
    }

    // Level controls are 0 to 100 %, squared: 100 % is unity, 70 % about -6 dB, 0 switches the voice off.
    static float LevelToGain(float percent)
    {
        if (!(percent > 0.0f)) return 0.0f;
        if (percent > 100.0f) percent = 100.0f;
        return percent * percent * 1e-4f;
    }

    // One hop of audio.
    //
    // Normally the dry signal goes straight to the output, undelayed and untouched, and Attack,
    // Filter and Detune work on the octave voices only. With Dry FX the dry signal takes the
    // voices' route: through the attack and the filter, and if Detune is up, through the engine
    // as a slightly flat unison voice (which also gives it the voices' latency).
    void Process(const float *in, float *out, const Settings& set)
    {
        const float cents = set.detune > 0.0f ? set.detune : 0.0f;
        const bool dryDetuned = set.dryFx && cents > 0.0f;

        // Up 1 sharp, Up 2 flat by the same amount, the dry signal a third of it flat; the sub
        // octaves stay in tune, a detuned bass only wobbles.
        float gains[OctaveEngine::NVOICES], detune[OctaveEngine::NVOICES];
        gains[OctaveEngine::SUB2] = set.voices[0];   detune[OctaveEngine::SUB2] = 1.0f;
        gains[OctaveEngine::SUB1] = set.voices[1];   detune[OctaveEngine::SUB1] = 1.0f;
        gains[OctaveEngine::UP1] = set.voices[2];    detune[OctaveEngine::UP1] = pow(2.0f, cents/1200.0f);
        gains[OctaveEngine::UP2] = set.voices[3];    detune[OctaveEngine::UP2] = pow(2.0f, -cents/1200.0f);
        gains[OctaveEngine::UNISON] = dryDetuned ? set.dry : 0.0f;
        detune[OctaveEngine::UNISON] = pow(2.0f, -cents/3600.0f);

        swell->Process(in, swellGain, hop, set.attack);
        engine->Process(in, wet, gains, detune);

        // Low-pass: a state-variable filter (trapezoidal integrators), which can have its cutoff
        // moved while running.
        double cutoff = set.cutoff;
        if (cutoff < 100.0) cutoff = 100.0;
        if (cutoff > 0.45*SampleRate) cutoff = 0.45*SampleRate;
        cutoff_1 += 0.25*(cutoff - cutoff_1);
        double q = set.resonance;
        if (q < 0.5) q = 0.5;
        if (q > 10.0) q = 10.0;
        const double g = tan(M_PI*cutoff_1/SampleRate);
        const double k = 1.0/q;
        const double a1 = 1.0/(1.0 + g*(g + k));
        const double a2 = g*a1;
        const double a3 = g*a2;

        // the dry signal after the filter (normal), or before it (Dry FX, not detuned)
        const float dryPost = set.dryFx ? 0.0f : set.dry;
        const float dryPre = set.dryFx && !dryDetuned ? set.dry : 0.0f;
        const float postStep = (dryPost - dryPost_1)/hop;
        const float preStep = (dryPre - dryPre_1)/hop;

        for (uint32_t i = 0; i < hop; i++)
        {
            dryPost_1 += postStep;
            dryPre_1 += preStep;

            const double v0 = swellGain[i]*(wet[i] + dryPre_1*in[i]);
            const double v3 = v0 - ic2;
            const double v1 = a1*ic1 + a2*v3;
            const double v2 = ic2 + a2*ic1 + a3*v3;
            ic1 = 2.0*v1 - ic1;
            ic2 = 2.0*v2 - ic2;

            out[i] = dryPost_1*in[i] + (float)v2;
        }
        dryPost_1 = dryPost;
        dryPre_1 = dryPre;

        if (fabs(ic1) < 1e-20) ic1 = 0;
        if (fabs(ic2) < 1e-20) ic2 = 0;
    }

    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *ports[PLUGIN_PORT_COUNT];

    OctaveEngine *engines[NTRACKING];
    OctaveEngine *engine;
    BlockAdapter *adapter;
    Swell *swell;
    float *wet;
    float *swellGain;

    int tracking;
    uint32_t hop;
    double SampleRate;
    float dryPost_1, dryPre_1;
    double cutoff_1;
    double ic1, ic2;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    PolyOctaver::instantiate,
    PolyOctaver::connect_port,
    PolyOctaver::activate,
    PolyOctaver::run,
    PolyOctaver::deactivate,
    PolyOctaver::cleanup,
    PolyOctaver::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle PolyOctaver::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    PolyOctaver *plugin = new PolyOctaver(samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void PolyOctaver::activate(LV2_Handle instance)
{
    PolyOctaver *plugin = (PolyOctaver *) instance;
    plugin->engine->Reset();
    plugin->swell->Reset();
    plugin->ic1 = plugin->ic2 = 0;
}

/**********************************************************************************************************************************************************/

void PolyOctaver::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void PolyOctaver::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    PolyOctaver *plugin;
    plugin = (PolyOctaver *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void PolyOctaver::run(LV2_Handle instance, uint32_t n_samples)
{
    PolyOctaver *plugin;
    plugin = (PolyOctaver *) instance;

    const float *in       = plugin->ports[IN];
    float       *out      = plugin->ports[OUT];
    Settings set;
    set.dry       = LevelToGain(*(plugin->ports[DRY]));
    set.voices[0] = LevelToGain(*(plugin->ports[SUB_2]));
    set.voices[1] = LevelToGain(*(plugin->ports[SUB_1]));
    set.voices[2] = LevelToGain(*(plugin->ports[UP_1]));
    set.voices[3] = LevelToGain(*(plugin->ports[UP_2]));
    set.attack    = *(plugin->ports[ATTACK]);
    set.cutoff    = *(plugin->ports[FILTER]);
    set.resonance = *(plugin->ports[RESONANCE]);
    set.detune    = *(plugin->ports[DETUNE]);
    set.dryFx     = *(plugin->ports[DRY_FX]) > 0.5f;
    const int    tracking = (int)(*(plugin->ports[TRACKING])+0.5f);

    plugin->SelectTracking(tracking);

    // The engine runs on a fixed hop. Host blocks that are a whole number of hops (128 and 256 on
    // the MOD devices) go straight through; anything else goes through the adapter, one hop late.
    const uint32_t hop = plugin->hop;
    if (!plugin->adapter->Buffered() && n_samples % hop == 0)
    {
        for (uint32_t off = 0; off < n_samples; off += hop)
            plugin->Process(in + off, out + off, set);
        return;
    }

    float* const outs[1] = { out };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], set);
    });
}

/**********************************************************************************************************************************************************/

void PolyOctaver::cleanup(LV2_Handle instance)
{
    delete ((PolyOctaver *) instance);
}

/**********************************************************************************************************************************************************/

const void* PolyOctaver::extension_data(const char* uri)
{
    return NULL;
}
