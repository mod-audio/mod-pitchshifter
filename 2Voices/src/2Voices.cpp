#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "PitchDetectionClass.h"
#include "HarmonizerLib.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/2Voices"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT_1, OUT_2, STEP_1, STEP_2, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class TwoVoices
{
public:
    TwoVoices(uint32_t n_samples, int nBuffers, double samplerate){Construct(n_samples, nBuffers, samplerate);}
    ~TwoVoices(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, double samplerate)
    {
        this->nBuffers = nBuffers;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers);
        objs_1 = new PSSinthesis(obja);
        objs_2 = new PSSinthesis(obja);
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);

        cont = 0;
        s_1 = 0;
        s_2 = 0;
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
        Construct(n_samples, nBuffers, SampleRate);
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
    float *out_2;
    float *step_1;
    float *step_2;
    float *gain_1;
    float *gain_2;
    
    PSAnalysis *obja;
    PSSinthesis *objs_1;
    PSSinthesis *objs_2;
    GainClass *objg1;
    GainClass *objg2;

    int nBuffers;
    int cont;
    double SampleRate;
	   
    double s_1;
    double s_2;
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
    TwoVoices *plugin = new TwoVoices(N_SAMPLES_DEFAULT, 32, samplerate);	
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void TwoVoices::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void TwoVoices::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

/**********************************************************************************************************************************************************/

void TwoVoices::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    TwoVoices *plugin;
    plugin = (TwoVoices *) instance;

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case OUT_2:
            plugin->out_2 = (float*) data;
            break;
        case STEP_1:
            plugin->step_1 = (float*) data;
            break;
        case STEP_2:
            plugin->step_2 = (float*) data;
            break;
        case GAIN_1:
            plugin->gain_1 = (float*) data;
            break;
        case GAIN_2:
            plugin->gain_2 = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void TwoVoices::run(LV2_Handle instance, uint32_t n_samples)
{
    TwoVoices *plugin;
    plugin = (TwoVoices *) instance;
    
    if ( (plugin->obja)->hopa != (int)n_samples )
    {
    	int nbuffers;
		
		switch ((int)n_samples)
		{
			case 64:
				nbuffers = 32;
				break;
			case 128:
				nbuffers = 16;
				break;
			case 256:
				nbuffers = 8;
				break;
			default:
				nbuffers = 4;
		}
        plugin->Realloc(n_samples, nbuffers);
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
			plugin->out_2[i] = 0;
		}
	}
	else
	{
       	plugin->s_1 = (double)(*(plugin->step_1));
       	plugin->s_2 = (double)(*(plugin->step_2));

        (plugin->objg1)->SetGaindB((double)(*(plugin->gain_1)));
        (plugin->objg2)->SetGaindB((double)(*(plugin->gain_2)));
        
        (plugin->obja)->PreAnalysis(plugin->nBuffers, plugin->in);
        (plugin->objs_1)->PreSinthesis();
        (plugin->objs_2)->PreSinthesis();
		
		if ( plugin->cont < plugin->nBuffers-1)
		{
			plugin->cont = plugin->cont + 1;
		}
		else
		{
            (plugin->obja)->Analysis();
            (plugin->objs_1)->Sinthesis(plugin->s_1);
            (plugin->objs_2)->Sinthesis(plugin->s_2);

            (plugin->objg1)->SimpleGain((plugin->objs_1)->yshift, plugin->out_1);
            (plugin->objg2)->SimpleGain((plugin->objs_2)->yshift, plugin->out_2);
		}
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
