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

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/Harmonizer"
#define FIDELITY0 4,2,1,1
#define FIDELITY1 8,4,2,1
#define FIDELITY2 12,6,3,2
#define FIDELITYPD 8,4,2,1
enum {IN, OUT_1, OUT_2, TONE, SCALE, INTERVAL, MODE, LOWNOTE, GAIN_1, GAIN_2, FIDELITY, PLUGIN_PORT_COUNT};

#define NFIDELITY 3

/**********************************************************************************************************************************************************/

class Harmonizer
{
public:
    Harmonizer(uint32_t n_samples, double samplerate, const std::string& wfile)
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
    ~Harmonizer()
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
    void Process(const float *in, float *out_1, float *out_2, int Tone, int Scale, int Interval, int Mode, int LowNote, double gain_1, double gain_2)
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
            FindStep(objpd->note, objpd->oitava, Tone, Scale, Interval, Mode, LowNote, &s);
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
    Harmonizer::instantiate,
    Harmonizer::connect_port,
    Harmonizer::activate,
    Harmonizer::run,
    Harmonizer::deactivate,
    Harmonizer::cleanup,
    Harmonizer::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle Harmonizer::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    Harmonizer *plugin = new Harmonizer(n_samples, samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void Harmonizer::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Harmonizer::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Harmonizer::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Harmonizer *plugin;
    plugin = (Harmonizer *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void Harmonizer::run(LV2_Handle instance, uint32_t n_samples)
{
    Harmonizer *plugin;
    plugin = (Harmonizer *) instance;

    const float *in       = plugin->ports[IN];
    float       *out_1    = plugin->ports[OUT_1];
    float       *out_2    = plugin->ports[OUT_2];
    int          Tone     = (int)(*(plugin->ports[TONE])+0.5f);
    int          Scale    = (int)(*(plugin->ports[SCALE])+0.5f);
    int          Interval = (int)(*(plugin->ports[INTERVAL])+0.5f);
    int          Mode     = (int)(*(plugin->ports[MODE])+0.5f);
    int          LowNote  = (int)(*(plugin->ports[LOWNOTE])+0.5f);
    double       gain_1   = (double)(*(plugin->ports[GAIN_1]));
    double       gain_2   = (double)(*(plugin->ports[GAIN_2]));
    int          fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SelectFidelity(fidelity);

    float* const outs[2] = { out_1, out_2 };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], houts[1], Tone, Scale, Interval, Mode, LowNote, gain_1, gain_2);
    });
}

/**********************************************************************************************************************************************************/

void Harmonizer::cleanup(LV2_Handle instance)
{
    delete ((Harmonizer *) instance);
}

/**********************************************************************************************************************************************************/

const void* Harmonizer::extension_data(const char* uri)
{
    return NULL;
}
