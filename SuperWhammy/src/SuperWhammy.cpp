#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/SuperWhammy"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT_1, STEP, FIRST, LAST, CLEAN, GAIN, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class SuperWhammy
{
public:
    SuperWhammy(uint32_t n_samples, int nBuffers, double samplerate)
    {
    	this->nBuffers = nBuffers;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers);
        objs = new PSSinthesis(obja);
        objg = new GainClass(n_samples);

        cont = 0;
        s = 0;
    }
    ~SuperWhammy()
    {
    	delete obja;
        delete objs;
        delete objg; 
    }

    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    //Ports
    float *in;
    float *out_1;
    float *step;
    float *first;
    float *last;
    float *clean;
    float *gain;
    
    PSAnalysis *obja;
    PSSinthesis *objs;
    GainClass *objg;

    int nBuffers;
    int cont;
    double SampleRate;
	   
    double s;
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
    SuperWhammy *plugin = new SuperWhammy(N_SAMPLES_DEFAULT, 34, samplerate);
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

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case STEP:
            plugin->step = (float*) data;
            break;
        case FIRST:
            plugin->first = (float*) data;
            break;
        case LAST:
            plugin->last = (float*) data;
            break;
        case CLEAN:
            plugin->clean = (float*) data;
            break;
        case GAIN:
            plugin->gain = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void SuperWhammy::run(LV2_Handle instance, uint32_t n_samples)
{
    SuperWhammy *plugin;
    plugin = (SuperWhammy *) instance;
    
    if ( (((plugin->obja)->hopa) != (int)n_samples) )
    {
    	int nbuffers;
        double samplerate = plugin->SampleRate;
		
		switch ((int)n_samples)
		{
			case 64:
				nbuffers = 34;
				break;
			case 128:
				nbuffers = 18;
				break;
			case 256:
				nbuffers = 10;
				break;
			case 512:
				nbuffers = 8;
				break;
		}
		
		delete plugin;
        plugin = new SuperWhammy(n_samples, nbuffers, samplerate);			
		return;
	}
 
    float sum_abs = 0;
    
    for (uint32_t i=0; i<n_samples; i++)
    {
		sum_abs = sum_abs + abs(plugin->in[i]);
	}
	
	if (sum_abs == 0)
	{
		for (uint32_t i=0; i<n_samples; i++)
		{
			plugin->out_1[i] = 0;
		}
	}
	else
	{
		
    plugin->s = (double)(*(plugin->step));
	(plugin->objg)->SetGaindB((double)(*(plugin->gain)));
    
    double a = (double)(*(plugin->first));
    double b = (double)(*(plugin->last));
    double c = (double)(*(plugin->clean));  
    double s_ = a + plugin->s*(b-a);
    
    (plugin->obja)->PreAnalysis(plugin->nBuffers, plugin->in);
    (plugin->objs)->PreSinthesis();
		
		if ( plugin->cont < plugin->nBuffers-1)
		{
			plugin->cont = plugin->cont + 1;
		}
		else
		{
 			(plugin->obja)->Analysis();
            (plugin->objs)->Sinthesis(s_);
            (plugin->objg)->SimpleGain((plugin->obja)->frames, plugin->out_1);
		}
		
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
