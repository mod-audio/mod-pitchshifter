#include <stdlib.h>
#include <cmath>
#include "PitchShifterClasses.h"
#include "GainClass.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/Capo"
#define FIDELITY0 4,2,1,1
#define FIDELITY1 12,6,3,2
#define FIDELITY2 20,10,5,3
enum {IN, OUT, STEP, GAIN, FIDELITY, PLUGIN_PORT_COUNT};

/**********************************************************************************************************************************************************/

class Capo
{
public:
    Capo(uint32_t n_samples, int nBuffers, double samplerate, const std::string& wfile)
    {
        wisdomFile = wfile;
        Construct(n_samples, nBuffers, samplerate, wfile.c_str());
    }
    ~Capo(){Destruct();}
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

    void SetFidelity(int fidelity, uint32_t n_samples)
    {
        int bufsize;

        switch (fidelity)
        {
        case 0:
            bufsize = nBuffersSW(n_samples,FIDELITY0);
            break;
        case 1:
            bufsize = nBuffersSW(n_samples,FIDELITY1);
            break;
        case 2:
            bufsize = nBuffersSW(n_samples,FIDELITY2);
            break;
        default:
            return;
        }

        if (nBuffers != bufsize || obja->hopa != (int)n_samples)
            Realloc(n_samples, bufsize);
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
    Capo::instantiate,
    Capo::connect_port,
    Capo::activate,
    Capo::run,
    Capo::deactivate,
    Capo::cleanup,
    Capo::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle Capo::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    const uint32_t n_samples = GetBufferSize(features);
    Capo *plugin = new Capo(n_samples, nBuffersSW(n_samples,FIDELITY1), samplerate, wisdomFile);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void Capo::activate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Capo::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void Capo::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Capo *plugin;
    plugin = (Capo *) instance;
    plugin->ports[port] = (float*) data;   
}

/**********************************************************************************************************************************************************/

void Capo::run(LV2_Handle instance, uint32_t n_samples)
{
    Capo *plugin;
    plugin = (Capo *) instance;
    
    float *in       = plugin->ports[IN];
    float *out      = plugin->ports[OUT];
    double s        = (double)(*(plugin->ports[STEP]));
    double gain     = (double)(*(plugin->ports[GAIN]));
    int    fidelity = (int)(*(plugin->ports[FIDELITY])+0.5f);

    plugin->SetFidelity(fidelity, n_samples);

    if (InputAbsSum(in, n_samples) == 0)
    {
        memset(out,0,sizeof(float)*n_samples);
        return;
    }
     
    (plugin->objg)->SetGaindB(gain);
    (plugin->obja)->PreAnalysis(plugin->nBuffers, in);
    (plugin->objs)->PreSinthesis();
            
    if (plugin->cont < plugin->nBuffers-1)
        plugin->cont = plugin->cont + 1;
    else
    {
        (plugin->obja)->Analysis();
        (plugin->objs)->Sinthesis(s);
        (plugin->objg)->SimpleGain((plugin->objs)->yshift, out);
    }
}

/**********************************************************************************************************************************************************/

void Capo::cleanup(LV2_Handle instance)
{
    delete ((Capo *) instance);
}

/**********************************************************************************************************************************************************/

const void* Capo::extension_data(const char* uri)
{
    return NULL;
}