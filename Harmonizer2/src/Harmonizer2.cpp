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

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/Harmonizer2"
#define FIDELITY0 4,2,1,1
#define FIDELITY1 8,4,2,1
#define FIDELITY2 12,6,3,2
#define FIDELITYPD 8,4,2,1
enum {IN, OUT_CLEAN, OUT_1, OUT_2, TONE, SCALE, INTERVAL_1, INTERVAL_2, MODE, LOWNOTE, GAIN_CLEAN, GAIN_1, GAIN_2, FIDELITY, PLUGIN_PORT_COUNT};

#define NFIDELITY 3

/**********************************************************************************************************************************************************/

class Harmonizer2
{
public:
    Harmonizer2(uint32_t n_samples, double samplerate, const std::string& wfile)
    {
        hop = n_samples;
        SampleRate = samplerate;
        nBuffers2 = nBuffersSW(n_samples,FIDELITYPD);
        s_1 = 0;
        s_2 = 0;

        // Everything that depends on the fidelity is built here, once, for every
        // fidelity: selecting one later is a pointer swap, safe on the audio thread.
        const bool haveWisdom = ImportWisdom(wfile.c_str());
        for (int f = 0; f < NFIDELITY; f++)
        {
            obja_all[f] = new PSAnalysis(n_samples, nBuffersFor(f, n_samples), haveWisdom);
            objs_1_all[f] = new PSSinthesis(obja_all[f], haveWisdom);
            objs_2_all[f] = new PSSinthesis(obja_all[f], haveWisdom);
        }
        objpd = new PitchDetection(n_samples, nBuffers2, samplerate, haveWisdom);
        objgc = new GainClass(n_samples);
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);
        adapter = new BlockAdapter(n_samples, 3);

        fidelity = -1;
        SelectFidelity(1);
    }
    ~Harmonizer2()
    {
        for (int f = 0; f < NFIDELITY; f++)
        {
            delete obja_all[f];
            delete objs_1_all[f];
            delete objs_2_all[f];
        }
        delete objpd;
        delete objgc;
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
        objs_1 = objs_1_all[f];
        objs_2 = objs_2_all[f];
        nBuffers = obja->Qcolumn;

        obja->Reset();
        objs_1->Reset();
        objs_2->Reset();
        objpd->Reset();
        cont = 0;
    }

    // One hop of audio: `in` and the outputs hold exactly `hop` samples.
    void Process(const float *in, float *out_clean, float *out_1, float *out_2, int Tone, int Scale, int Interval_1, int Interval_2, int Mode, int LowNote, double gain_clean, double gain_1, double gain_2)
    {
        if (InputAbsSum(in, hop) == 0)
        {
            memset(out_clean,0,sizeof(float)*hop);
            memset(out_1,0,sizeof(float)*hop);
            memset(out_2,0,sizeof(float)*hop);
            return;
        }

        objgc->SetGaindB(gain_clean);
        objg1->SetGaindB(gain_1);
        objg2->SetGaindB(gain_2);

        obja->PreAnalysis(nBuffers, in);
        objs_1->PreSinthesis();
        objs_2->PreSinthesis();
        objpd->PreProcessing(nBuffers2, in);

        if (cont < nBuffers-1)
        {
            cont = cont + 1;
            memset(out_clean,0,sizeof(float)*hop);
            memset(out_1,0,sizeof(float)*hop);
            memset(out_2,0,sizeof(float)*hop);
        }
        else
        {
            objpd->FindNote();
            FindStep(objpd->note, objpd->oitava, Tone, Scale, Interval_1, Mode, LowNote, &s_1);
            FindStep(objpd->note, objpd->oitava, Tone, Scale, Interval_2, Mode, LowNote, &s_2);
            obja->Analysis();
            objs_1->Sinthesis(s_1);
            objs_2->Sinthesis(s_2);
            objgc->SimpleGain(obja->frames, out_clean);
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
    PitchDetection *objpd;
    GainClass *objgc;
    GainClass *objg1;
    GainClass *objg2;
    BlockAdapter *adapter;

    int nBuffers;
    int nBuffers2;
    int cont;
    int fidelity;
    uint32_t hop;
    double SampleRate;
    double s_1;
    double s_2;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    Harmonizer2::instantiate,
    Harmonizer2::connect_port,
    Harmonizer2::activate,
    Harmonizer2::run,
    Harmonizer2::deactivate,
    Harmonizer2::cleanup,
    Harmonizer2::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle Harmonizer2::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    Harmonizer2 *plugin = new Harmonizer2(n_samples, samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void Harmonizer2::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Harmonizer2::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Harmonizer2::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Harmonizer2 *plugin;
    plugin = (Harmonizer2 *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void Harmonizer2::run(LV2_Handle instance, uint32_t n_samples)
{
    Harmonizer2 *plugin;
    plugin = (Harmonizer2 *) instance;

    const float *in         = plugin->ports[IN];
    float       *out_clean  = plugin->ports[OUT_CLEAN];
    float       *out_1      = plugin->ports[OUT_1];
    float       *out_2      = plugin->ports[OUT_2];
    int          Tone       = (int)(*(plugin->ports[TONE])+0.5f);
    int          Scale      = (int)(*(plugin->ports[SCALE])+0.5f);
    int          Interval_1 = (int)(*(plugin->ports[INTERVAL_1])+0.5f);
    int          Interval_2 = (int)(*(plugin->ports[INTERVAL_2])+0.5f);
    int          Mode       = (int)(*(plugin->ports[MODE])+0.5f);
    int          LowNote    = (int)(*(plugin->ports[LOWNOTE])+0.5f);
    double       gain_clean = (double)(*(plugin->ports[GAIN_CLEAN]));
    double       gain_1     = (double)(*(plugin->ports[GAIN_1]));
    double       gain_2     = (double)(*(plugin->ports[GAIN_2]));
    int          fidelity   = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SelectFidelity(fidelity);

    float* const outs[3] = { out_clean, out_1, out_2 };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], houts[1], houts[2], Tone, Scale, Interval_1, Interval_2, Mode, LowNote, gain_clean, gain_1, gain_2);
    });
}

/**********************************************************************************************************************************************************/

void Harmonizer2::cleanup(LV2_Handle instance)
{
    delete ((Harmonizer2 *) instance);
}

/**********************************************************************************************************************************************************/

const void* Harmonizer2::extension_data(const char* uri)
{
    return NULL;
}
