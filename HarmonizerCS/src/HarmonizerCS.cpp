#include <stdlib.h>
#include <cmath>
#include <lv2.h>
#include "PitchShifterClasses.h"
#include "PitchDetectionClass.h"
#include "HarmonizerLib.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/HarmonizerCS"
#define N_SAMPLES_DEFAULT 128
enum {IN, OUT_1, OUT_2, TONE, STEP_0, STEP_1, STEP_2, STEP_3, STEP_4, STEP_5, STEP_6, STEP_7, STEP_8, STEP_9, STEP_10, STEP_11, LOWNOTE, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class HarmonizerCS
{
public:
    HarmonizerCS(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate, const std::string& wfile)
    {
        wisdomFile = wfile;
        Construct(n_samples, nBuffers, nBuffers2, samplerate, wfile.c_str());
    }
    ~HarmonizerCS(){Destruct();}
    void Construct(uint32_t n_samples, int nBuffers, int nBuffers2, double samplerate, const char* wisdomFile)
    {
        this->nBuffers = nBuffers;
        this->nBuffers2 = nBuffers2;
        SampleRate = samplerate;

        obja = new PSAnalysis(n_samples, nBuffers, wisdomFile);
        objs = new PSSinthesis(obja, wisdomFile);
        objpd = new PitchDetection(n_samples, nBuffers2, samplerate, wisdomFile);
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
        Construct(n_samples, nBuffers, nBuffers2, SampleRate, wisdomFile.c_str());
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
    PitchDetection *objpd;
    GainClass *objg1;
    GainClass *objg2;

    int nBuffers;
    int nBuffers2;
    int cont;
    double SampleRate;
    double s;
    std::string wisdomFile;
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
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    HarmonizerCS *plugin = new HarmonizerCS(N_SAMPLES_DEFAULT, 32, 16, samplerate, wisdomFile);
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
    plugin->ports[port] = (float*) data;
}

/**********************************************************************************************************************************************************/

void HarmonizerCS::run(LV2_Handle instance, uint32_t n_samples)
{
    HarmonizerCS *plugin;
    plugin = (HarmonizerCS *) instance;

    float *in     = plugin->ports[IN];
    float *out_1  = plugin->ports[OUT_1];
    float *out_2  = plugin->ports[OUT_2];
    int Tone      = (int)(*(plugin->ports[TONE]));
    int LowNote   = (int)(*(plugin->ports[LOWNOTE]));
    int s_0       = (int)(*(plugin->ports[STEP_0]));
    int s_1       = (int)(*(plugin->ports[STEP_1]));
    int s_2       = (int)(*(plugin->ports[STEP_2]));
    int s_3       = (int)(*(plugin->ports[STEP_3]));
    int s_4       = (int)(*(plugin->ports[STEP_4]));
    int s_5       = (int)(*(plugin->ports[STEP_5]));
    int s_6       = (int)(*(plugin->ports[STEP_6]));
    int s_7       = (int)(*(plugin->ports[STEP_7]));
    int s_8       = (int)(*(plugin->ports[STEP_8]));
    int s_9       = (int)(*(plugin->ports[STEP_9]));
    int s_10      = (int)(*(plugin->ports[STEP_10]));
    int s_11      = (int)(*(plugin->ports[STEP_11]));
    double gain_1 = (double)(*(plugin->ports[GAIN_1]));
    double gain_2 = (double)(*(plugin->ports[GAIN_2]));

    if ( (plugin->obja)->hopa != (int)n_samples )
    {
        plugin->Realloc(n_samples, nBuffersSW(n_samples,32,16,8,4), nBuffersSW(n_samples,16,8,4,2));
        return;
	}
	
	if (InputAbsSum(in, n_samples) == 0)
	{
        fill_n(out_1,n_samples,0);
        fill_n(out_2,n_samples,0);
        return;
	}

	(plugin->objg1)->SetGaindB(gain_1);
    (plugin->objg2)->SetGaindB(gain_2);
    (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
    (plugin->objs)->PreSinthesis();
    (plugin->objpd)->PreProcessing(plugin->nBuffers2, in);
	
	if ( plugin->cont < plugin->nBuffers-1)
		plugin->cont = plugin->cont + 1;
	else
	{
		(plugin->objpd)->FindNote();
		FindStepCS((plugin->objpd)->note, (plugin->objpd)->oitava, Tone, LowNote, s_0, s_1, s_2, s_3, s_4, s_5, s_6, s_7, s_8, s_9, s_10, s_11, &plugin->s);
		
		(plugin->obja)->Analysis();
        (plugin->objs)->Sinthesis(plugin->s);
        (plugin->objg1)->SimpleGain((plugin->obja)->frames, out_1);
        (plugin->objg2)->SimpleGain((plugin->objs)->yshift, out_2);			
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