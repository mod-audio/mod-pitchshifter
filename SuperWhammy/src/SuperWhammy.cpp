#include <stdlib.h>
#include <string.h>
#include <string>
#include <cmath>
#include "PitchShifterClasses.h"
#include "GainClass.h"
#include "BlockAdapter.h"

#undef IN
#undef OUT

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/SuperWhammy"
#define FIDELITY0 6,3,2,1
#define FIDELITY1 12,6,3,2
#define FIDELITY2 16,8,4,2
#define FIDELITY3 20,10,5,3
#define SMOOTH_DURATION 0.1
enum {IN, OUT, STEP, FIRST, LAST, CLEAN, GAIN, FIDELITY, PLUGIN_PORT_COUNT};

#define NFIDELITY 4

/**********************************************************************************************************************************************************/

class SuperWhammy
{
public:
    SuperWhammy(uint32_t n_samples, double samplerate, const std::string& wfile)
    {
        hop = n_samples;
        SampleRate = samplerate;
        current_s = 0.0;

        // Everything that depends on the fidelity is built here, once, for every
        // fidelity: selecting one later is a pointer swap, safe on the audio thread.
        const bool haveWisdom = ImportWisdom(wfile.c_str());
        for (int f = 0; f < NFIDELITY; f++)
        {
            obja_all[f] = new PSAnalysis(n_samples, nBuffersFor(f, n_samples), haveWisdom);
            objs_all[f] = new PSSinthesis(obja_all[f], haveWisdom);
        }
        objg = new GainClass(n_samples);
        adapter = new BlockAdapter(n_samples, 1);

        fidelity = -1;
        SelectFidelity(1);
    }
    ~SuperWhammy()
    {
        for (int f = 0; f < NFIDELITY; f++)
        {
            delete obja_all[f];
            delete objs_all[f];
        }
        delete objg;
        delete adapter;
    }

    static int nBuffersFor(int fidelity, uint32_t n_samples)
    {
        switch (fidelity)
        {
        case 0:  return nBuffersSW(n_samples,FIDELITY0);
        case 1:  return nBuffersSW(n_samples,FIDELITY1);
        case 2:  return nBuffersSW(n_samples,FIDELITY2);
        case 3:  return nBuffersSW(n_samples,FIDELITY3);
        default: return nBuffersSW(n_samples,FIDELITY1);
        }
    }

    // Switch to the pre-built engine for fidelity `f`; out-of-range values keep the
    // current one. Only pointer swaps and history resets: no allocation, no FFTW planning.
    void SelectFidelity(int f)
    {
        if (f < 0 || f >= NFIDELITY || f == fidelity)
            return;

        fidelity = f;
        obja = obja_all[f];
        objs = objs_all[f];
        nBuffers = obja->Qcolumn;

        obja->Reset();
        objs->Reset();
        cont = 0;
    }

    // One hop of audio: `in` and the outputs hold exactly `hop` samples.
    void Process(const float *in, float *out, double s, double gain, double a, double b, int c)
    {
        if (InputAbsSum(in, hop) == 0)
        {
            memset(out,0,sizeof(float)*hop);
            return;
        }

        double buffer_duration = hop/((double) SampleRate);
        double alpha = std::pow(0.01, buffer_duration/SMOOTH_DURATION);
        current_s = alpha*current_s + (1-alpha)*s;

        double s_ = a + current_s*(b-a);
        objg->SetGaindB(gain);
        obja->PreAnalysis(nBuffers, in);
        objs->PreSinthesis();

        if (cont < nBuffers-1)
        {
            cont = cont + 1;
            memset(out,0,sizeof(float)*hop);
        }
        else
        {
            obja->Analysis();
            objs->Sinthesis(s_);
            objg->SimpleGain(objs->yshift, out);
            if (c == 1) for (uint32_t i = 0; i<hop; i++) out[i] += obja->frames[i];
        }
    }

    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *ports[PLUGIN_PORT_COUNT];

    PSAnalysis *obja_all[NFIDELITY];
    PSAnalysis *obja;
    PSSinthesis *objs_all[NFIDELITY];
    PSSinthesis *objs;
    GainClass *objg;
    BlockAdapter *adapter;

    int nBuffers;
    int cont;
    int fidelity;
    uint32_t hop;
    double SampleRate;
    double current_s;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    SuperWhammy::instantiate,
    SuperWhammy::connect_port,
    SuperWhammy::activate,
    SuperWhammy::run,
    SuperWhammy::deactivate,
    SuperWhammy::cleanup,
    SuperWhammy::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle SuperWhammy::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    SuperWhammy *plugin = new SuperWhammy(n_samples, samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void SuperWhammy::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void SuperWhammy::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void SuperWhammy::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    SuperWhammy *plugin;
    plugin = (SuperWhammy *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void SuperWhammy::run(LV2_Handle instance, uint32_t n_samples)
{
    SuperWhammy *plugin;
    plugin = (SuperWhammy *) instance;

    const float *in       = plugin->ports[IN];
    float       *out      = plugin->ports[OUT];
    double       s        = (double)(*(plugin->ports[STEP]));
    double       gain     = (double)(*(plugin->ports[GAIN]));
    double       a        = (double)(*(plugin->ports[FIRST]));
    double       b        = (double)(*(plugin->ports[LAST]));
    int          c        = (int)(*(plugin->ports[CLEAN])+0.5f);
    int          fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SelectFidelity(fidelity);

    float* const outs[1] = { out };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], s, gain, a, b, c);
    });
}

/**********************************************************************************************************************************************************/

void SuperWhammy::cleanup(LV2_Handle instance)
{
    delete ((SuperWhammy *) instance);
}

/**********************************************************************************************************************************************************/

const void* SuperWhammy::extension_data(const char* uri)
{
    return NULL;
}
