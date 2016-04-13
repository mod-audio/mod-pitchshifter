#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/SuperWhammy"
#define N_SAMPLES_DEFAULT 128
enum {IN, OUT, STEP, FIRST, LAST, CLEAN, GAIN, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class SuperWhammy
{
public:
    SuperWhammy(uint32_t n_samples, int nBuffers, double samplerate, const std::string& wfile)
    {
        wisdomFile = wfile;
        Construct(n_samples, nBuffers, samplerate, wfile.c_str());
    }
    ~SuperWhammy(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, double samplerate, const char* wisdomFile)
    {
    	this->nBuffers = nBuffers;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers, wisdomFile);
        objs = new PSSinthesis(obja, wisdomFile);
        objg = new GainClass(n_samples);

        cont = 0;
    }
    void Destruct()
    {
    	delete obja;
        delete objs;
        delete objg;
    }
    void Realloc(uint32_t n_samples, int nBuffers)
    {
    	Destruct();
    	Construct(n_samples, nBuffers, SampleRate, wisdomFile.c_str());
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

    int nBuffers;
    int cont;
    double SampleRate;
    std::string wisdomFile;
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
    SuperWhammy *plugin = new SuperWhammy(N_SAMPLES_DEFAULT, nBuffersSW(N_SAMPLES_DEFAULT*2,34,18,12,8), samplerate, wisdomFile);
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

    float *in   = plugin->ports[IN];
    float *out  = plugin->ports[OUT];
    double s    = (double)(*(plugin->ports[STEP]));
    double gain = (double)(*(plugin->ports[GAIN]));
    double a    = (double)(*(plugin->ports[FIRST]));
    double b    = (double)(*(plugin->ports[LAST]));
    int c       = (int)(*(plugin->ports[CLEAN]));
    
    if ( (plugin->obja)->hopa != (int)n_samples )
    {
        plugin->Realloc(n_samples, nBuffersSW(n_samples*2,34,18,12,8));
		return;
	}

    if (InputAbsSum(in, n_samples) == 0)
    {
        fill_n(out,n_samples,0);
        return;
    }

    double s_ = a + s*(b-a);
	(plugin->objg)->SetGaindB(gain);    
    (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
    (plugin->objs)->PreSinthesis();
	
	if (plugin->cont < plugin->nBuffers-1)
		plugin->cont = plugin->cont + 1;
	else
	{
        (plugin->obja)->Analysis();
        (plugin->objs)->Sinthesis(s_);
        (plugin->objg)->SimpleGain((plugin->objs)->yshift, out);
        if (c == 1) for (uint32_t i = 0; i<n_samples; i++) out[i] += (plugin->obja)->frames[i];
	}	
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