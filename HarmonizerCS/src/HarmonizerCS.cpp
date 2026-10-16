#include <stdlib.h>
#include <string.h>
#include <string>
#include <cmath>
#include "PitchShifterClasses.h"
#include "PitchDetectionClass.h"
#include "HarmonizerLib.h"
#include "GainClass.h"
#include "BlockAdapter.h"

#undef IN

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/HarmonizerCS"
#define FIDELITY0 4,2,1,1
#define FIDELITY1 8,4,2,1
#define FIDELITY2 12,6,3,2
#define FIDELITYPD 8,4,2,1
enum {IN, OUT_1, OUT_2, TONE, STEP_0, STEP_1, STEP_2, STEP_3, STEP_4, STEP_5, STEP_6, STEP_7, STEP_8, STEP_9, STEP_10, STEP_11, LOWNOTE, GAIN_1, GAIN_2, FIDELITY, PLUGIN_PORT_COUNT};

#define NFIDELITY 3

/**********************************************************************************************************************************************************/

class HarmonizerCS
{
public:
    HarmonizerCS(uint32_t n_samples, double samplerate, const std::string& wfile)
    {
        hop = n_samples;
        SampleRate = samplerate;
        nBuffers2 = nBuffersSW(n_samples,FIDELITYPD);
        s = 0;

        // Everything that depends on the fidelity is built here, once, for every
        // fidelity: selecting one later is a pointer swap, safe on the audio thread.
        const bool haveWisdom = ImportWisdom(wfile.c_str());
        for (int f = 0; f < NFIDELITY; f++)
        {
            obja_all[f] = new PSAnalysis(n_samples, nBuffersFor(f, n_samples), haveWisdom);
            objs_all[f] = new PSSinthesis(obja_all[f], haveWisdom);
        }
        objpd = new PitchDetection(n_samples, nBuffers2, samplerate, haveWisdom);
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);
        adapter = new BlockAdapter(n_samples, 2);

        fidelity = -1;
        SelectFidelity(1);
    }
    ~HarmonizerCS()
    {
        for (int f = 0; f < NFIDELITY; f++)
        {
            delete obja_all[f];
            delete objs_all[f];
        }
        delete objpd;
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
        objpd->Reset();
        cont = 0;
    }

    // One hop of audio: `in` and the outputs hold exactly `hop` samples.
    void Process(const float *in, float *out_1, float *out_2, int Tone, int LowNote, int s_0, int s_1, int s_2, int s_3, int s_4, int s_5, int s_6, int s_7, int s_8, int s_9, int s_10, int s_11, double gain_1, double gain_2)
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
        objs->PreSinthesis();
        objpd->PreProcessing(nBuffers2, in);

        if (cont < nBuffers-1)
        {
            cont = cont + 1;
            memset(out_1,0,sizeof(float)*hop);
            memset(out_2,0,sizeof(float)*hop);
        }
        else
        {
            objpd->FindNote();
            FindStepCS(objpd->note, objpd->oitava, Tone, LowNote, s_0, s_1, s_2, s_3, s_4, s_5, s_6, s_7, s_8, s_9, s_10, s_11, &s);

            obja->Analysis();
            objs->Sinthesis(s);
            objg1->SimpleGain(obja->frames, out_1);
            objg2->SimpleGain(objs->yshift, out_2);
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
    PitchDetection *objpd;
    GainClass *objg1;
    GainClass *objg2;
    BlockAdapter *adapter;

    int nBuffers;
    int nBuffers2;
    int cont;
    int fidelity;
    uint32_t hop;
    double SampleRate;
    double s;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    HarmonizerCS::instantiate,
    HarmonizerCS::connect_port,
    HarmonizerCS::activate,
    HarmonizerCS::run,
    HarmonizerCS::deactivate,
    HarmonizerCS::cleanup,
    HarmonizerCS::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle HarmonizerCS::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    HarmonizerCS *plugin = new HarmonizerCS(n_samples, samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void HarmonizerCS::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void HarmonizerCS::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void HarmonizerCS::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    HarmonizerCS *plugin;
    plugin = (HarmonizerCS *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void HarmonizerCS::run(LV2_Handle instance, uint32_t n_samples)
{
    HarmonizerCS *plugin;
    plugin = (HarmonizerCS *) instance;

    const float *in       = plugin->ports[IN];
    float       *out_1    = plugin->ports[OUT_1];
    float       *out_2    = plugin->ports[OUT_2];
    int          Tone     = (int)(*(plugin->ports[TONE])+0.5f);
    int          LowNote  = (int)(*(plugin->ports[LOWNOTE])+0.5f);
    int          s_0       = (int)(*(plugin->ports[STEP_0])+0.5f);
    int          s_1       = (int)(*(plugin->ports[STEP_1])+0.5f);
    int          s_2       = (int)(*(plugin->ports[STEP_2])+0.5f);
    int          s_3       = (int)(*(plugin->ports[STEP_3])+0.5f);
    int          s_4       = (int)(*(plugin->ports[STEP_4])+0.5f);
    int          s_5       = (int)(*(plugin->ports[STEP_5])+0.5f);
    int          s_6       = (int)(*(plugin->ports[STEP_6])+0.5f);
    int          s_7       = (int)(*(plugin->ports[STEP_7])+0.5f);
    int          s_8       = (int)(*(plugin->ports[STEP_8])+0.5f);
    int          s_9       = (int)(*(plugin->ports[STEP_9])+0.5f);
    int          s_10      = (int)(*(plugin->ports[STEP_10])+0.5f);
    int          s_11      = (int)(*(plugin->ports[STEP_11])+0.5f);
    double       gain_1   = (double)(*(plugin->ports[GAIN_1]));
    double       gain_2   = (double)(*(plugin->ports[GAIN_2]));
    int          fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SelectFidelity(fidelity);

    float* const outs[2] = { out_1, out_2 };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], houts[1], Tone, LowNote, s_0, s_1, s_2, s_3, s_4, s_5, s_6, s_7, s_8, s_9, s_10, s_11, gain_1, gain_2);
    });
}

/**********************************************************************************************************************************************************/

void HarmonizerCS::cleanup(LV2_Handle instance)
{
    delete ((HarmonizerCS *) instance);
}

/**********************************************************************************************************************************************************/

const void* HarmonizerCS::extension_data(const char* uri)
{
    return NULL;
}
