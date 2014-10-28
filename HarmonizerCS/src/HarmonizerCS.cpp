#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "PitchDetectionClass.h"
#include "HarmonizerLib.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/HarmonizerCS"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT_1, OUT_2, TONE, STEP_0, STEP_1, STEP_2, STEP_3, STEP_4, STEP_5, STEP_6, STEP_7, STEP_8, STEP_9, STEP_10, STEP_11, LOWNOTE, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class HarmonizerCS
{
public:
    HarmonizerCS(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate){Construct(n_samples, nBuffers, nBuffers2, samplerate);}
    ~HarmonizerCS(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate)
    {
        this->nBuffers = nBuffers;
        this->nBuffers2 = nBuffers2;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers);
        objs = new PSSinthesis(obja);
        objpd = new PitchDetection(n_samples, nBuffers2, samplerate);
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);

        cont = 0;
        s = 0;
    }
    void Destruct()
    {
        delete obja;
        delete objs;
        delete objpd;
        delete objg1;
        delete objg2;
    }
    void Realloc(uint32_t n_samples, int nBuffers, int nBuffers2)
    {
        double SampleRate = this->SampleRate;
        Destruct();
        Construct(n_samples, nBuffers, nBuffers2, SampleRate);
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
    float *tone;
    float *step_0;
    float *step_1;
    float *step_2;
    float *step_3;
    float *step_4;
    float *step_5;
    float *step_6;
    float *step_7;
    float *step_8;
    float *step_9;
    float *step_10;
    float *step_11;
    float *lownote;
    float *gain_1;
    float *gain_2;
    
    PSAnalysis *obja;
    PSSinthesis *objs;
    PitchDetection *objpd;
    GainClass *objg1;
    GainClass *objg2;

    int nBuffers;
    int nBuffers2;
    int cont;
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
    HarmonizerCS *plugin = new HarmonizerCS(N_SAMPLES_DEFAULT, 32, 16, samplerate);
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
        case TONE:
            plugin->tone = (float*) data;
            break;
        case STEP_0:
            plugin->step_0 = (float*) data;
            break;
        case STEP_1:
            plugin->step_1 = (float*) data;
            break;
        case STEP_2:
            plugin->step_2 = (float*) data;
            break;
        case STEP_3:
            plugin->step_3 = (float*) data;
            break;
        case STEP_4:
            plugin->step_4 = (float*) data;
            break;
        case STEP_5:
            plugin->step_5 = (float*) data;
            break;
        case STEP_6:
            plugin->step_6 = (float*) data;
            break;
        case STEP_7:
            plugin->step_7 = (float*) data;
            break;
        case STEP_8:
            plugin->step_8 = (float*) data;
            break;
        case STEP_9:
            plugin->step_9 = (float*) data;
            break;
        case STEP_10:
            plugin->step_10 = (float*) data;
            break;
        case STEP_11:
            plugin->step_11 = (float*) data;
            break;
        case LOWNOTE:
            plugin->lownote = (float*) data;
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

void HarmonizerCS::run(LV2_Handle instance, uint32_t n_samples)
{
    HarmonizerCS *plugin;
    plugin = (HarmonizerCS *) instance;
    
    if ( (plugin->obja)->hopa != (int)n_samples )
    {
    	int nbuffers;
        int nbuffers2;
		
		switch ((int)n_samples)
		{
			case 64:
				nbuffers = 32;
				nbuffers2 = 16;
				break;
			case 128:
				nbuffers = 16;
				nbuffers2 = 8;
				break;
			case 256:
				nbuffers = 8;
				nbuffers2 = 4;
				break;
			default:
				nbuffers = 4;
				nbuffers2 = 2;
		}
		plugin->Realloc(n_samples, nbuffers, nbuffers2);
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
		int Tone = (int)(*(plugin->tone));
		int LowNote = (int)(*(plugin->lownote));

		int s_0 = (int)(*(plugin->step_0));
		int s_1 = (int)(*(plugin->step_1));
		int s_2 = (int)(*(plugin->step_2));
		int s_3 = (int)(*(plugin->step_3));
		int s_4 = (int)(*(plugin->step_4));
		int s_5 = (int)(*(plugin->step_5));
		int s_6 = (int)(*(plugin->step_6));
		int s_7 = (int)(*(plugin->step_7));
		int s_8 = (int)(*(plugin->step_8));
		int s_9 = (int)(*(plugin->step_9));
		int s_10 = (int)(*(plugin->step_10));
		int s_11 = (int)(*(plugin->step_11));
			    
		(plugin->objg1)->SetGaindB((double)(*(plugin->gain_1)));
        (plugin->objg2)->SetGaindB((double)(*(plugin->gain_2)));

        (plugin->obja)->PreAnalysis(plugin->nBuffers, plugin->in);
        (plugin->objs)->PreSinthesis();
        (plugin->objpd)->PreProcessing(plugin->nBuffers2, plugin->in);
		
		if ( plugin->cont < plugin->nBuffers-1)
		{
			plugin->cont = plugin->cont + 1;
		}
		else
		{
			(plugin->objpd)->FindNote();
			FindStepCS((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, LowNote, s_0, s_1, s_2, s_3, s_4, s_5, s_6, s_7, s_8, s_9, s_10, s_11, &plugin->s);
			
			(plugin->obja)->Analysis();
            (plugin->objs)->Sinthesis(plugin->s);

            (plugin->objg1)->SimpleGain((plugin->obja)->frames, plugin->out_1);
            (plugin->objg2)->SimpleGain((plugin->objs)->yshift, plugin->out_2);			
		}
		
	}

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