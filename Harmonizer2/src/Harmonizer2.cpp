#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "PitchDetectionClass.h"
#include "HarmonizerLib.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Harmonizer2"
#define N_SAMPLES_DEFAULT 64
enum {IN, OUT_CLEAN, OUT_1, OUT_2, TONE, SCALE, INTERVAL_1, INTERVAL_2, MODE, LOWNOTE, GAIN_CLEAN, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class Harmonizer2
{
public:
    Harmonizer2(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate){Construct(n_samples, nBuffers, nBuffers2, samplerate);}
    ~Harmonizer2(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate)
    {
        this->nBuffers = nBuffers;
        this->nBuffers2 = nBuffers2;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers);
        objs_1 = new PSSinthesis(obja);
        objs_2 = new PSSinthesis(obja);
        objpd = new PitchDetection(n_samples, nBuffers2, samplerate);
        objgc = new GainClass(n_samples);
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
        delete objpd;
        delete objgc;
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
    float *ports[PLUGIN_PORT_COUNT];

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
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void Harmonizer2::run(LV2_Handle instance, uint32_t n_samples)
{
    Harmonizer2 *plugin;
    plugin = (Harmonizer2 *) instance;

    float *in         = plugin->ports[IN];
    float *out_clean  = plugin->ports[OUT_CLEAN];
    float *out_1      = plugin->ports[OUT_1];
    float *out_2      = plugin->ports[OUT_2];
    int Tone          = (int)(*(plugin->ports[TONE]));
    int Scale         = (int)(*(plugin->ports[SCALE]));
    int Interval_1    = (int)(*(plugin->ports[INTERVAL_1]));
    int Interval_2    = (int)(*(plugin->ports[INTERVAL_2]));
    int Mode          = (int)(*(plugin->ports[MODE]));
    int LowNote       = (int)(*(plugin->ports[LOWNOTE]));
    double gain_clean = (double)(*(plugin->ports[GAIN_CLEAN]));
    double gain_1     = (double)(*(plugin->ports[GAIN_1]));
    double gain_2     = (double)(*(plugin->ports[GAIN_2]));
    
    if ( (plugin->obja)->hopa != (int)n_samples )
    {
        plugin->Realloc(n_samples, nBuffersSW(n_samples,32,16,8,4), nBuffersSW(n_samples,16,8,4,2));
        return;
	}

    float sum_abs = 0;
    
    for (uint32_t i=0; i<n_samples; i++)
		sum_abs = sum_abs + abs(in[i]);
	
	if (sum_abs == 0)
	{
        fill_n(out_clean,n_samples,0);
        fill_n(out_1,n_samples,0);
        fill_n(out_2,n_samples,0);
	}
	else
	{
        (plugin->objgc)->SetGaindB(gain_clean);
        (plugin->objg1)->SetGaindB(gain_1);
        (plugin->objg2)->SetGaindB(gain_2);
        
        (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
        (plugin->objs_1)->PreSinthesis();
        (plugin->objs_2)->PreSinthesis();
        (plugin->objpd)->PreProcessing(plugin->nBuffers2, in);
		
		if ( plugin->cont < plugin->nBuffers-1)
			plugin->cont = plugin->cont + 1;
		else
		{
			(plugin->objpd)->FindNote();
            FindStep((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval_1, Mode, LowNote, &plugin->s_1);
            FindStep((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, Scale, Interval_2, Mode, LowNote, &plugin->s_2);

            (plugin->obja)->Analysis();
            (plugin->objs_1)->Sinthesis(plugin->s_1);
            (plugin->objs_2)->Sinthesis(plugin->s_2);

            (plugin->objgc)->SimpleGain((plugin->obja)->frames, out_clean);
            (plugin->objg1)->SimpleGain((plugin->objs_1)->yshift, out_1);
            (plugin->objg2)->SimpleGain((plugin->objs_2)->yshift, out_2);
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