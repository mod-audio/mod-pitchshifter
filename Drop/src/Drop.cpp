#include <stdlib.h>
#include <cmath>
#include "PitchShifterClasses.h"
#include "GainClass.h"
#include "PitchSplicer.h"   // "Ultra": correlation-spliced time-domain shifter (no FFT)

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/Drop"
#define FIDELITY0 4,2,1,1
#define FIDELITY1 8,4,2,1
#define FIDELITY2 16,8,4,2
enum {IN, OUT, STEP, GAIN, FIDELITY, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class Drop
{
public:
    Drop(uint32_t n_samples, int nBuffers, double samplerate, const std::string& wfile)
    {
        wisdomFile = wfile;
        Construct(n_samples, nBuffers, samplerate, wfile.c_str());
    }
    ~Drop(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, double samplerate, const char* wisdomFile)
    {
    	this->nBuffers = nBuffers;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers, wisdomFile);
        objs = new PSSinthesis(obja, wisdomFile);
        objg = new GainClass(n_samples);
        // "Ultra" fidelity engine: sized once for the rate; per-sample and never reallocs after
        // prepare(), so host block size is irrelevant to it (unlike the STFT objects above).
        objp = new PitchSplicer();
        objp->prepare(SampleRate);
        objp->setEnabled(true);

        cont = 0;
    }
    void Destruct()
    {
    	delete obja;
        delete objs;
        delete objg;
        delete objp;
    }
    void Realloc(uint32_t n_samples, int nBuffers)
    {
    	Destruct();
    	Construct(n_samples, nBuffers, SampleRate, wisdomFile.c_str());
    }

    void SetFidelity(int fidelity, uint32_t n_samples)
    {
        int bufsize;

        switch (fidelity)
        {
        case 0:
            bufsize = nBuffersSW(n_samples,FIDELITY0);
            break;
        case 1:
            bufsize = nBuffersSW(n_samples,FIDELITY1);
            break;
        case 2:
            bufsize = nBuffersSW(n_samples,FIDELITY2);
            break;
        case 3:   // "Ultra": correlation-spliced engine (objp). Nothing to re-size - its state is
                    // independent of host block size, so the STFT objects are left untouched.
            return;
        default:
            return;
        }

        if (nBuffers != bufsize || obja->hopa != (int)n_samples)
            Realloc(n_samples, bufsize);
    }

    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    float *ports[PLUGIN_PORT_COUNT];
    
    PSAnalysis *obja;
    PSSinthesis *objs;
    GainClass *objg;
    PitchSplicer *objp;   // "Ultra" fidelity engine (correlation-spliced, time-domain)

    int nBuffers;
    int cont;
    double SampleRate;
    std::string wisdomFile;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    Drop::instantiate,
    Drop::connect_port,
    Drop::activate,
    Drop::run,
    Drop::deactivate,
    Drop::cleanup,
    Drop::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle Drop::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    Drop *plugin = new Drop(n_samples, nBuffersSW(n_samples,FIDELITY1), samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void Drop::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Drop::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Drop::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Drop *plugin;
    plugin = (Drop *) instance;
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/


void Drop::run(LV2_Handle instance, uint32_t n_samples)
{
    Drop *plugin;
    plugin = (Drop *) instance;

    float *in       = plugin->ports[IN];
    float *out      = plugin->ports[OUT];
    double s        = (double)(*(plugin->ports[STEP]));
    double gain     = (double)(*(plugin->ports[GAIN]));
    int    fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SetFidelity(fidelity, n_samples);
    if (fidelity == 3)   // "Ultra": routed before the silence early-out on purpose - even when
          // the input is silent the ring must keep running so it decays out smoothly instead of hard-cutting to zero.
    {
        PitchSplicer::Params p;
        p.semitones = static_cast<float>(s);   // STEP port: whole semitones (continuous also fine)
        plugin->objp->setParams(p);            // window stays at the default set in prepare()

        const float* ins[1]  = { in };
        float*       outs[1] = { out };
        plugin->objp->process(ins, outs, static_cast<int>(n_samples), 1);   // shifted audio into `out` (pre-gain)

        (plugin->objg)->SetGaindB(gain);
        (plugin->objg)->SimpleGain(out, out);   // GAIN port applied in-place to the shift result
        return;
    }


    if (InputAbsSum(in, n_samples) == 0)
    {
        memset(out,0,sizeof(float)*n_samples);
        return;
    }

    (plugin->objg)->SetGaindB(gain);
    (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
    (plugin->objs)->PreSinthesis();

    if (plugin->cont < plugin->nBuffers-1)
        plugin->cont = plugin->cont + 1;
    else
    {
        (plugin->obja)->Analysis();
        (plugin->objs)->Sinthesis(s);
        (plugin->objg)->SimpleGain((plugin->objs)->yshift, out);
    }
}

/**********************************************************************************************************************************************************/

void Drop::cleanup(LV2_Handle instance)
{
    delete ((Drop *) instance);
}

/**********************************************************************************************************************************************************/

const void* Drop::extension_data(const char* uri)
{
    return NULL;
}