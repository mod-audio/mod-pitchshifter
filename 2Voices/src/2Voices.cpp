#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/2Voices"
#define N_SAMPLES_DEFAULT 128
#define FIDELITY0 6,3,2,1
#define FIDELITY1 12,6,3,2
#define FIDELITY2 16,8,4,2
#define FIDELITY3 20,10,5,3
enum {IN, OUT_1, OUT_2, STEP_1, STEP_2, GAIN_1, GAIN_2, FIDELITY, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class TwoVoices
{
public:
    TwoVoices(uint32_t n_samples, int nBuffers, double samplerate, const std::string& wfile)
    {
        wisdomFile = wfile;
        Construct(n_samples, nBuffers, samplerate, wfile.c_str());
    }
    ~TwoVoices(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, double samplerate, const char* wisdomFile)
    {
        this->nBuffers = nBuffers;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers, wisdomFile);
        objs_1 = new PSSinthesis(obja, wisdomFile);
        objs_2 = new PSSinthesis(obja, wisdomFile);
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);

        cont = 0;
    }
    void Destruct()
    {
        delete obja;
        delete objs_1;
        delete objs_2;
        delete objg1;
        delete objg2;
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
        case 3:
            bufsize = nBuffersSW(n_samples,FIDELITY3);
            break;
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
    PSSinthesis *objs_1;
    PSSinthesis *objs_2;
    GainClass *objg1;
    GainClass *objg2;

    int nBuffers;
    int cont;
    double SampleRate;
    std::string wisdomFile;
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
    TwoVoices *plugin = new TwoVoices(N_SAMPLES_DEFAULT, nBuffersSW(N_SAMPLES_DEFAULT,FIDELITY1), samplerate, wisdomFile);
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

    float *in       = plugin->ports[IN];
    float *out_1    = plugin->ports[OUT_1];
    float *out_2    = plugin->ports[OUT_2];
    double s_1      = (double)(*(plugin->ports[STEP_1]));
    double s_2      = (double)(*(plugin->ports[STEP_2]));
    double gain_1   = (double)(*(plugin->ports[GAIN_1]));
    double gain_2   = (double)(*(plugin->ports[GAIN_2]));
    int    fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SetFidelity(fidelity, n_samples);

    if (InputAbsSum(in, n_samples) == 0)
    {
        fill_n(out_1,n_samples,0);
        fill_n(out_2,n_samples,0);
        return;
    }

    (plugin->objg1)->SetGaindB(gain_1);
    (plugin->objg2)->SetGaindB(gain_2);
    
    (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
    (plugin->objs_1)->PreSinthesis();
    (plugin->objs_2)->PreSinthesis();
	
	if (plugin->cont < plugin->nBuffers-1)
		plugin->cont = plugin->cont + 1;
	else
	{
        (plugin->obja)->Analysis();
        (plugin->objs_1)->Sinthesis(s_1);
        (plugin->objs_2)->Sinthesis(s_2);
        (plugin->objg1)->SimpleGain((plugin->objs_1)->yshift, out_1);
        (plugin->objg2)->SimpleGain((plugin->objs_2)->yshift, out_2);
	}
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