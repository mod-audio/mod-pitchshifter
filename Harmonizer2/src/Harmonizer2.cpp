#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "shift.h"
#include "PitchDetection.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Harmonizer2"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT_CLEAN, OUT_1, OUT_2, TONE, SCALE, INTERVAL_1, INTERVAL_2, MODE, LOWNOTE, GAIN_CLEAN, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class Harmonizer2
{
public:
    Harmonizer2(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate) 
    {
        this->nBuffers = nBuffers;
        this->nBuffers2 = nBuffers2;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers);
        objs_1 = new PSSinthesis(obja);
        objs_2 = new PSSinthesis(obja);
        objpd = new PitchDetection(n_samples, nBuffers2);
        objgc = new GainClass(n_samples);
        objg1 = new GainClass(n_samples);
        objg2 = new GainClass(n_samples);

        cont = 0;
        s_1 = 0;
        s_2 = 0;
    }
    ~Harmonizer2()
    {
        delete obja;
        delete objs_1;
        delete objs_2;
        delete objpd;
        delete objgc;
        delete objg1;
        delete objg2;   
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
    float *out_clean;
    float *out_1;
    float *out_2;
    float *tone;
    float *scale;
    float *interval_1;
    float *interval_2;
    float *mode;
    float *lownote;
    float *gain_clean;
    float *gain_1;
    float *gain_2;

    PSAnalysis *obja;
    PSSinthesis *objs_1;
    PSSinthesis *objs_2;
    PitchDetection *objpd;
    GainClass *objgc;
    GainClass *objg1;
    GainClass *objg2;

    int nBuffers;
    int nBuffers2;
    int cont;
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
    Harmonizer2 *plugin = new Harmonizer2(N_SAMPLES_DEFAULT, 32, 16, samplerate);
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

    switch (port)
    {
        case IN:
            plugin->in = (float*) data;
            break;
        case OUT_CLEAN:
            plugin->out_clean = (float*) data;
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
        case SCALE:
            plugin->scale = (float*) data;
            break;
        case INTERVAL_1:
            plugin->interval_1 = (float*) data;
            break;
        case INTERVAL_2:
            plugin->interval_2 = (float*) data;
            break;
        case MODE:
            plugin->mode = (float*) data;
            break;
        case LOWNOTE:
            plugin->lownote = (float*) data;
            break;
        case GAIN_CLEAN:
            plugin->gain_clean = (float*) data;
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

void Harmonizer2::run(LV2_Handle instance, uint32_t n_samples)
{
    Harmonizer2 *plugin;
    plugin = (Harmonizer2 *) instance;
    
    if ( (((plugin->obja)->hopa) != (int)n_samples) )
    {
        int nbuffers;
        int nbuffers2;
        double samplerate = plugin->SampleRate;
		
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

        delete plugin;
        plugin = new Harmonizer2(n_samples, nbuffers, nbuffers2, samplerate);		
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
    	int Scale = (int)(*(plugin->scale));
    	int Interval_1 = (int)(*(plugin->interval_1));
    	int Interval_2 = (int)(*(plugin->interval_2));
    	int Mode = (int)(*(plugin->mode));
    	int LowNote = (int)(*(plugin->lownote));

        (plugin->objgc)->SetGaindB((double)(*(plugin->gain_clean)));
        (plugin->objg1)->SetGaindB((double)(*(plugin->gain_1)));
        (plugin->objg2)->SetGaindB((double)(*(plugin->gain_2)));
        
        (plugin->obja)->PreAnalysis(plugin->nBuffers, plugin->in);
        (plugin->objs_1)->PreSinthesis();
        (plugin->objs_2)->PreSinthesis();
        (plugin->objpd)->PreProcessing(plugin->nBuffers2, plugin->in);
		
		if ( plugin->cont < plugin->nBuffers-1)
		{
			plugin->cont = plugin->cont + 1;
		}
		else
		{
			(plugin->objpd)->FindNote();
            FindStep2((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval_1, Mode, LowNote, &plugin->s_1);
            FindStep2((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval_2, Mode, LowNote, &plugin->s_2);

            (plugin->obja)->Analysis();
            (plugin->objs_1)->Sinthesis(plugin->s_1);
            (plugin->objs_2)->Sinthesis(plugin->s_2);

            (plugin->objgc)->SimpleGain((plugin->obja)->frames, plugin->out_clean);
            (plugin->objg1)->SimpleGain((plugin->objs_1)->yshift, plugin->out_1);
            (plugin->objg2)->SimpleGain((plugin->objs_2)->yshift, plugin->out_2);
			
		}
		
	}

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