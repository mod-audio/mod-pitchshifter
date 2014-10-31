#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "PitchDetectionClass.h"
#include "HarmonizerLib.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Harmonizer"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT_1, OUT_2, TONE, SCALE, INTERVAL, MODE, LOWNOTE, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class Harmonizer
{
public:
    Harmonizer(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate){Construct(n_samples, nBuffers, nBuffers2, samplerate);}
    ~Harmonizer(){Destruct();}
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
    float *scale;
    float *interval;
    float *mode;
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
	Harmonizer *plugin = new Harmonizer(N_SAMPLES_DEFAULT, 32, 16, samplerate);
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
        case SCALE:
            plugin->scale = (float*) data;
            break;
        case INTERVAL:
            plugin->interval = (float*) data;
            break;
        case MODE:
            plugin->mode = (float*) data;
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

void Harmonizer::run(LV2_Handle instance, uint32_t n_samples)
{
    Harmonizer *plugin;
    plugin = (Harmonizer *) instance;
    
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
    	int Scale = (int)(*(plugin->scale));
    	int Interval = (int)(*(plugin->interval));
    	int Mode = (int)(*(plugin->mode));
    	int LowNote = (int)(*(plugin->lownote));

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
            FindStep((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval, Mode, LowNote, &plugin->s);

            (plugin->obja)->Analysis();
            (plugin->objs)->Sinthesis(plugin->s);

            (plugin->objg1)->SimpleGain((plugin->obja)->frames, plugin->out_1);
            (plugin->objg2)->SimpleGain((plugin->objs)->yshift, plugin->out_2);
		}
	}
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