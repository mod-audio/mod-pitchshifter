#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "shift.h"
#include "PitchDetection.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Harmonizer2"
#define TAMANHO_DO_BUFFER 64
enum {IN, OUT_CLEAN, OUT_1, OUT_2, TONE, SCALE, INTERVAL_1, INTERVAL_2, MODE, LOWNOTE, GAIN_CLEAN, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class PitchShifter
{
public:
    PitchShifter() {}
    ~PitchShifter() {}
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
    PitchShifter::instantiate,
    PitchShifter::connect_port,
    PitchShifter::activate,
    PitchShifter::run,
    PitchShifter::deactivate,
    PitchShifter::cleanup,
    PitchShifter::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle PitchShifter::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    PitchShifter *plugin = new PitchShifter();
    
    plugin->SampleRate = samplerate;
        
    plugin->nBuffers = 32;
    plugin->nBuffers2 = 16;

    plugin->obja = new PSAnalysis(TAMANHO_DO_BUFFER, plugin->nBuffers);
    plugin->objs_1 = new PSSinthesis(plugin->obja);
    plugin->objs_2 = new PSSinthesis(plugin->obja);
    plugin->objpd = new PitchDetection(TAMANHO_DO_BUFFER, plugin->nBuffers2);
    plugin->objgc = new GainClass(TAMANHO_DO_BUFFER);
    plugin->objg1 = new GainClass(TAMANHO_DO_BUFFER);
    plugin->objg2 = new GainClass(TAMANHO_DO_BUFFER);

    plugin->cont = 0;

    plugin->s_1 = 0;
    plugin->s_2 = 0;
	
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void PitchShifter::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void PitchShifter::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void PitchShifter::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    PitchShifter *plugin;
    plugin = (PitchShifter *) instance;

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

void PitchShifter::run(LV2_Handle instance, uint32_t n_samples)
{
    PitchShifter *plugin;
    plugin = (PitchShifter *) instance;
    
    if ( (((plugin->obja)->hopa) != (int)n_samples) )
    {
		
		switch ((int)n_samples)
		{
			case 64:
				plugin->nBuffers = 32;
				plugin->nBuffers2 = 16;
				break;
			case 128:
				plugin->nBuffers = 16;
				plugin->nBuffers2 = 8;
				break;
			case 256:
				plugin->nBuffers = 8;
				plugin->nBuffers2 = 4;
				break;
			case 512:
				plugin->nBuffers = 4;
				plugin->nBuffers2 = 2;
				break;
		}

        delete plugin->obja;
        delete plugin->objs_1;
        delete plugin->objs_2;
        delete plugin->objpd;
        delete plugin->objgc;
        delete plugin->objg1;
        delete plugin->objg2;

        plugin->obja = new PSAnalysis(n_samples, plugin->nBuffers);
        plugin->objs_1 = new PSSinthesis(plugin->obja);
        plugin->objs_2 = new PSSinthesis(plugin->obja);
        plugin->objpd = new PitchDetection(n_samples, plugin->nBuffers2);
        plugin->objgc = new GainClass(n_samples);
        plugin->objg1 = new GainClass(n_samples);
        plugin->objg2 = new GainClass(n_samples);
		
		return;
	}

    float media = 0;
    
    for (uint32_t i=1; i<=n_samples; i++)
    {
		media = media + abs(plugin->in[i-1]);
	}
	
	if (media == 0)
	{
		for (uint32_t i=1; i<=n_samples; i++)
		{
			plugin->out_1[i-1] = 0;
			plugin->out_2[i-1] = 0;
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

    (plugin->objgc)->g = pow(10, (float)(*(plugin->gain_clean))/20.0);
    (plugin->objg1)->g = pow(10, (float)(*(plugin->gain_1))/20.0);
    (plugin->objg2)->g = pow(10, (float)(*(plugin->gain_2))/20.0);
    
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
			FindStep((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval_1, Mode, LowNote, (plugin->obja)->hopa, (plugin->obja)->Qcolumn, &plugin->s_1, (plugin->objs_1)->hops);
			FindStep((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval_2, Mode, LowNote, (plugin->obja)->hopa, (plugin->obja)->Qcolumn, &plugin->s_2, (plugin->objs_2)->hops);

            (plugin->obja)->Analysis();
            (plugin->objs_1)->Sinthesis();
            (plugin->objs_2)->Sinthesis();

            (plugin->objgc)->SimpleGain((plugin->obja)->frames, plugin->out_clean);
            (plugin->objgc)->SimpleGain((plugin->objs_1)->yshift, plugin->out_1);
            (plugin->objgc)->SimpleGain((plugin->objs_2)->yshift, plugin->out_2);
			
		}
		
	}

}

/**********************************************************************************************************************************************************/

void PitchShifter::cleanup(LV2_Handle instance)
{
	PitchShifter *plugin;
	plugin = (PitchShifter *) instance;

    delete plugin->obja;
    delete plugin->objs_1;
    delete plugin->objs_2;
    delete plugin->objpd;
    delete plugin->objgc;
    delete plugin->objg1;
    delete plugin->objg2;
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
