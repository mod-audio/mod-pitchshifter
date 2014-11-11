#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Drop"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT, STEP, GAIN, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class Drop
{
public:
    Drop(uint32_t n_samples, int nBuffers, double samplerate){Construct(n_samples, nBuffers, samplerate);}
    ~Drop(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, double samplerate)
    {
    	this->nBuffers = nBuffers;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers);
        objs = new PSSinthesis(obja);
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
    	Construct(n_samples, nBuffers, SampleRate);
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
    Drop *plugin = new Drop(N_SAMPLES_DEFAULT, 16, samplerate);
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

    float *in = plugin->ports[IN];
	float *out = plugin->ports[OUT];
	double s = (double)(*(plugin->ports[STEP]));
	double gain = (double)(*(plugin->ports[GAIN]));
    
    if ( (plugin->obja)->hopa != (int)n_samples )
    {
    	int nbuffers;

		switch ((int)n_samples)
		{
			case 64:
				nbuffers = 16;
				break;
			case 128:
				nbuffers = 8;
				break;
			case 256:
				nbuffers = 4;
				break;
			default:
				nbuffers = 3;
		}
        plugin->Realloc(n_samples, nbuffers);
		return;
	}

    float sum_abs = 0;

    for (uint32_t i=0; i<n_samples; i++)
		sum_abs = sum_abs + abs(in[i]);
	
	if (sum_abs == 0)
		fill_n(out,n_samples,0);
	else
	{
        (plugin->objg)->SetGaindB(gain);
        (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
        (plugin->objs)->PreSinthesis();
	    	
		if ( plugin->cont < plugin->nBuffers-1)
			plugin->cont = plugin->cont + 1;
		else
		{
            (plugin->obja)->Analysis();
            (plugin->objs)->Sinthesis(s);
            (plugin->objg)->SimpleGain((plugin->objs)->yshift, out);
		}
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