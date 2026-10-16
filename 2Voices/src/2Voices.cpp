#include <stdlib.h>
#include <string.h>
#include <string>
#include <cmath>
#include "PitchShifterClasses.h"
#include "GainClass.h"
#include "BlockAdapter.h"

#undef IN

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/2Voices"
#define FIDELITY0 6,3,2,1
#define FIDELITY1 12,6,3,2
#define FIDELITY2 16,8,4,2
#define FIDELITY3 20,10,5,3
enum {IN, OUT_1, OUT_2, STEP_1, STEP_2, GAIN_1, GAIN_2, FIDELITY, PLUGIN_PORT_COUNT};

#define NFIDELITY 4

/**********************************************************************************************************************************************************/

class TwoVoices
{
public:
    TwoVoices(uint32_t n_samples, double samplerate, const std::string& wfile)
    {
        hop = n_samples;
        SampleRate = samplerate;

        // Everything that depends on the fidelity is built here, once, for every
        // fidelity: selecting one later is a pointer swap, safe on the audio thread.
        const bool haveWisdom = ImportWisdom(wfile.c_str());
        for (int f = 0; f < NFIDELITY; f++)
        {
            obja_all[f] = new PSAnalysis(n_samples, nBuffersFor(f, n_samples), haveWisdom);
            objs_1_all[f] = new PSSinthesis(obja_all[f], haveWisdom);
            objs_2_all[f] = new PSSinthesis(obja_all[f], haveWisdom);
        }
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);
        adapter = new BlockAdapter(n_samples, 2);

        fidelity = -1;
        SelectFidelity(1);
    }
    ~TwoVoices()
    {
        for (int f = 0; f < NFIDELITY; f++)
        {
            delete obja_all[f];
            delete objs_1_all[f];
            delete objs_2_all[f];
        }
        delete objg1;
        delete objg2;
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
        objs_1 = objs_1_all[f];
        objs_2 = objs_2_all[f];
        nBuffers = obja->Qcolumn;

        obja->Reset();
        objs_1->Reset();
        objs_2->Reset();
        cont = 0;
    }

    // One hop of audio: `in` and the outputs hold exactly `hop` samples.
    void Process(const float *in, float *out_1, float *out_2, double s_1, double s_2, double gain_1, double gain_2)
    {
        if (InputAbsSum(in, hop) == 0)
        {
            memset(out_1,0,sizeof(float)*hop);
            memset(out_2,0,sizeof(float)*hop);
            return;
        }

        objg1->SetGaindB(gain_1);
        objg2->SetGaindB(gain_2);

        obja->PreAnalysis(nBuffers, in);
        objs_1->PreSinthesis();
        objs_2->PreSinthesis();

        if (cont < nBuffers-1)
        {
            cont = cont + 1;
            memset(out_1,0,sizeof(float)*hop);
            memset(out_2,0,sizeof(float)*hop);
        }
        else
        {
            obja->Analysis();
            objs_1->Sinthesis(s_1);
            objs_2->Sinthesis(s_2);
            objg1->SimpleGain(objs_1->yshift, out_1);
            objg2->SimpleGain(objs_2->yshift, out_2);
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
    PSSinthesis *objs_1_all[NFIDELITY];
    PSSinthesis *objs_1;
    PSSinthesis *objs_2_all[NFIDELITY];
    PSSinthesis *objs_2;
    GainClass *objg1;
    GainClass *objg2;
    BlockAdapter *adapter;

    int nBuffers;
    int cont;
    int fidelity;
    uint32_t hop;
    double SampleRate;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    TwoVoices::instantiate,
    TwoVoices::connect_port,
    TwoVoices::activate,
    TwoVoices::run,
    TwoVoices::deactivate,
    TwoVoices::cleanup,
    TwoVoices::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle TwoVoices::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    TwoVoices *plugin = new TwoVoices(n_samples, samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void TwoVoices::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void TwoVoices::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void TwoVoices::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    TwoVoices *plugin;
    plugin = (TwoVoices *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void TwoVoices::run(LV2_Handle instance, uint32_t n_samples)
{
    TwoVoices *plugin;
    plugin = (TwoVoices *) instance;

    const float *in       = plugin->ports[IN];
    float       *out_1    = plugin->ports[OUT_1];
    float       *out_2    = plugin->ports[OUT_2];
    double       s_1      = (double)(*(plugin->ports[STEP_1]));
    double       s_2      = (double)(*(plugin->ports[STEP_2]));
    double       gain_1   = (double)(*(plugin->ports[GAIN_1]));
    double       gain_2   = (double)(*(plugin->ports[GAIN_2]));
    int          fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SelectFidelity(fidelity);

    float* const outs[2] = { out_1, out_2 };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], houts[1], s_1, s_2, gain_1, gain_2);
    });
}

/**********************************************************************************************************************************************************/

void TwoVoices::cleanup(LV2_Handle instance)
{
    delete ((TwoVoices *) instance);
}

/**********************************************************************************************************************************************************/

const void* TwoVoices::extension_data(const char* uri)
{
    return NULL;
}
