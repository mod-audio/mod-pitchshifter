#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include <complex>
#include "shift.h"
#include "window.h"
#include "angle.h"
#include <fftw3.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Drop"
#define TAMANHO_DO_BUFFER 1024
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
    double *XaPrevious_arg;
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
    double s;
    
    int *Hops;
    
    fftw_plan p;
	fftw_plan p2;
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
    plugin->Qcolumn = 32;
    plugin->nBuffers = 16;
    //Começam os testes
    plugin->hopa = TAMANHO_DO_BUFFER;
    plugin->N = plugin->nBuffers*TAMANHO_DO_BUFFER;
    plugin->Hops = (int*)malloc(plugin->Qcolumn*sizeof(int));
    plugin->w = (double*)malloc(plugin->N*sizeof(double));
    plugin->frames = (double*)malloc(plugin->N*sizeof(double));
    plugin->PhiPrevious = (double*)malloc(plugin->N*sizeof(double));
    plugin->XaPrevious = (complex<double>*)malloc(plugin->N*sizeof(complex<double>));
    plugin->XaPrevious_arg = (double*)malloc(plugin->N*sizeof(double));
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
		plugin->XaPrevious_arg[i-1] = 0;
		for (int k=1; k<=plugin->Qcolumn; k++)
		{
			plugin->Q[i-1][k-1] = 0;
		}
	}
	
	for (int k=1; k<=plugin->Qcolumn; k++)
	{
		plugin->Hops[k-1] = plugin->hopa;
	}
	
	plugin->p = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->framesaux), reinterpret_cast<fftw_complex*>(plugin->Xa), FFTW_FORWARD, FFTW_ESTIMATE);
	plugin->p2 = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->Xs), reinterpret_cast<fftw_complex*>(plugin->qaux), FFTW_BACKWARD, FFTW_ESTIMATE);
	
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
    int hops;
    double s_before = plugin->s;
    plugin->s = (double)(*(plugin->step));
    hops = round(plugin->hopa*(pow(2,(plugin->s/12))));
    
    if ((plugin->s != s_before)||(plugin->cont==0))
    {
		switch ( (int)(plugin->s))
		{
			case 2:
			plugin->nBuffers = 5;
			break;
			case 3:
			plugin->nBuffers = 6;
			break;
			case 4:
			plugin->nBuffers = 8;
			break;
			case 5:
			plugin->nBuffers = 9;
			break;
			case 6:
			plugin->nBuffers = 10;
			break;
			case 7:
			plugin->nBuffers = 10;
			break;
			case 8:
			plugin->nBuffers = 10;
			break;
			case 9:
			plugin->nBuffers = 10;
			break;
			case 10:
			plugin->nBuffers = 10;
			break;
			case 11:
			plugin->nBuffers = 11;
			break;
			case 12:
			plugin->nBuffers = 12;
			break;
			case 13:
			plugin->nBuffers = 12;
			break;
			case 14:
			plugin->nBuffers = 12;
			break;
			case 15:
			plugin->nBuffers = 12;
			break;
			case 16:
			plugin->nBuffers = 12;
			break;
			case 17:
			plugin->nBuffers = 12;
			break;
			case 18:
			plugin->nBuffers = 12;
			break;
			case 19:
			plugin->nBuffers = 12;
			break;
			case 20:
			plugin->nBuffers = 12;
			break;
			case 21:
			plugin->nBuffers = 12;
			break;
			case 22:
			plugin->nBuffers = 12;
			break;
			case 23:
			plugin->nBuffers = 12;
			break;
			case 24:
			plugin->nBuffers = 12;
			break;
			default:
			plugin->nBuffers = 4;
			break;
		}
			plugin->N = plugin->nBuffers*n_samples;
			hann2(plugin->N,plugin->w);
			fftw_destroy_plan(plugin->p);
			fftw_destroy_plan(plugin->p2);
			plugin->p = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->framesaux), reinterpret_cast<fftw_complex*>(plugin->Xa), FFTW_FORWARD, FFTW_ESTIMATE);
			plugin->p2 = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->Xs), reinterpret_cast<fftw_complex*>(plugin->qaux), FFTW_BACKWARD, FFTW_ESTIMATE);
			for (int i=1 ; i<= plugin->nBuffers; i++)
			{
			plugin->b[i-1] = &plugin->frames[(i-1)*plugin->hopa];
			}
	}
    
	for (int k=1; k<= plugin->Qcolumn-1; k++)
    {
		plugin->Hops[k-1] = plugin->Hops[k];
	}
    
    plugin->Hops[plugin->Qcolumn-1] = hops;
    
    if ( ((plugin->hopa) != (int)n_samples) )
    {
		plugin->hopa = n_samples;
		plugin->N = plugin->nBuffers*n_samples;
		hann2(plugin->N,plugin->w);
		fftw_destroy_plan(plugin->p);
		fftw_destroy_plan(plugin->p2);
		plugin->p = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->framesaux), reinterpret_cast<fftw_complex*>(plugin->Xa), FFTW_FORWARD, FFTW_ESTIMATE);
		plugin->p2 = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->Xs), reinterpret_cast<fftw_complex*>(plugin->qaux), FFTW_BACKWARD, FFTW_ESTIMATE);
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
			shift(plugin->N, plugin->hopa, plugin->Hops, plugin->frames, plugin->w, plugin->XaPrevious, plugin->XaPrevious_arg, plugin->PhiPrevious, plugin->Q, plugin->yshift, plugin->Xa, plugin->Xs, plugin->q, plugin->qaux, plugin->framesaux, plugin->Phi, plugin->ysaida, plugin->ysaida2,  plugin->Qcolumn, plugin->p, plugin->p2);
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
	free(plugin->XaPrevious_arg);
	
	fftw_destroy_plan(plugin->p);
	fftw_destroy_plan(plugin->p2);
	
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
