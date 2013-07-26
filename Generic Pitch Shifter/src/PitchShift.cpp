#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include <complex>
#include "shift.h"
#include "window.h"
#include <fftw3.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/PitchShifter"
#define TAMANHO_DO_BUFFER 512
enum {IN, OUT_1, STEP, PLUGIN_PORT_COUNT};

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
    float *in;
    float *out_1;
    float *step;
    
    int hopa;
    int N;
    double *w;
    double *frames;
    //double *b1, *b2, *b3, *b4, *b5, *b6, *b7, *b8;
    double **b;
    double *PhiPrevious;
    complex<double> *XaPrevious;
    double **Q;
    int cont;
    
    complex<double> *Xa;
    complex<double> *Xs;
    double *q;
    complex<double> *qaux;
    complex<double> *framesaux;
    double *Phi;
    double *ysaida;
    double *ysaida2;
    
    double *yshift;
    
    int Qcolumn;
    int nBuffers;
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
    plugin->Qcolumn = 16;
    plugin->nBuffers = 8;
    //Começam os testes
    plugin->hopa = TAMANHO_DO_BUFFER;
    plugin->N = plugin->nBuffers*TAMANHO_DO_BUFFER;
    plugin->w = (double*)malloc(plugin->N*sizeof(double));
    plugin->frames = (double*)malloc(plugin->N*sizeof(double));
    plugin->PhiPrevious = (double*)malloc(plugin->N*sizeof(double));
    plugin->XaPrevious = (complex<double>*)malloc(plugin->N*sizeof(complex<double>));
    plugin->Q = (double**)malloc(plugin->N*sizeof(double*));
    plugin->b = (double**)malloc(plugin->hopa*sizeof(double*));
    for (int i=1; i<=plugin->N; i++)
    {
		plugin->Q[i-1] = (double*)malloc(plugin->Qcolumn*sizeof(double));
	}
	
	plugin->Xa = (complex<double>*)malloc(plugin->N*sizeof(complex<double>));
	plugin->Xs = (complex<double>*)malloc(plugin->N*sizeof(complex<double>));
	plugin->q = (double*)malloc(plugin->N*sizeof(double));
	plugin->qaux = (complex<double>*)malloc(plugin->N*sizeof(complex<double>));
	plugin->framesaux = (complex<double>*)malloc(plugin->N*sizeof(complex<double>));
	plugin->Phi = (double*)malloc(plugin->N*sizeof(double));
	plugin->ysaida = (double*)malloc((plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa)*sizeof(double));
	
	plugin->yshift = (double*)malloc(plugin->hopa*sizeof(double));
	
    hann2(plugin->N,plugin->w);
    
    for (int i=1 ; i<= (plugin->nBuffers); i++)
    {
		plugin->b[i-1] = &plugin->frames[(i-1)*plugin->hopa];
	}

    plugin->cont = 0;
    for (int i=1;i<=plugin->N;i++)
    {
		plugin->frames[i-1] = 0;
		plugin->PhiPrevious[i-1] = 0;
		plugin->XaPrevious[i-1] = 0;
		for (int k=1; k<=plugin->Qcolumn; k++)
		{
			plugin->Q[i-1][k-1] = 0;
		}
	}
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void PitchShifter::activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/

void PitchShifter::deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}

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
        case OUT_1:
            plugin->out_1 = (float*) data;
            break;
        case STEP:
            plugin->step = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void PitchShifter::run(LV2_Handle instance, uint32_t n_samples)
{
    PitchShifter *plugin;
    plugin = (PitchShifter *) instance;
    /* double *pfOutput; */
    double s;
    int hops;
    s = (double)(*(plugin->step));
    hops = round(plugin->hopa*(pow(2,(s/12))));
    
    if ( ((plugin->hopa) != (int)n_samples) )
    {
		plugin->hopa = n_samples;
		plugin->N = plugin->nBuffers*n_samples;
		hann2(plugin->N,plugin->w);
		for (int i=1 ; i<= plugin->nBuffers; i++)
		{
			plugin->b[i-1] = &plugin->frames[(i-1)*plugin->hopa];
		}
	}
    
		for (int i=1; i<=plugin->hopa; i++)
		{
			for (int j=1; j<=(plugin->nBuffers-1); j++)
			{
				plugin->b[j-1][i-1] = plugin->b[j][i-1];
			}
			plugin->b[plugin->nBuffers-1][i-1] = plugin->in[i-1];
		}
		
		if ( plugin->cont < plugin->nBuffers-1)
		{
			plugin->cont = plugin->cont + 1;
		}
		else
		{
			shift(plugin->N, plugin->hopa, hops, plugin->frames, plugin->w, plugin->XaPrevious, plugin->PhiPrevious, plugin->Q, plugin->yshift, plugin->Xa, plugin->Xs, plugin->q, plugin->qaux, plugin->framesaux, plugin->Phi, plugin->ysaida, plugin->ysaida2,  plugin->Qcolumn);
			for (int i=1; i<=plugin->hopa; i++)
			{
				plugin->out_1[i-1] = (float)plugin->yshift[i-1];
			}
		}

}

/**********************************************************************************************************************************************************/

void PitchShifter::cleanup(LV2_Handle instance)
{
	PitchShifter *plugin;
	plugin = (PitchShifter *) instance;
	free(plugin->Xa);
	free(plugin->Xs);
	free(plugin->q);
	free(plugin->qaux);
	free(plugin->Phi);
	free(plugin->framesaux);
	free(plugin->ysaida);
	
	free(plugin->frames);
	free(plugin->w);
	free(plugin->Q);
	free(plugin->PhiPrevious);
	free(plugin->XaPrevious);
	
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
