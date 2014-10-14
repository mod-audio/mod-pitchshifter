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

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/SuperWhammy"
#define TAMANHO_DO_BUFFER 64
enum {IN, OUT_1, STEP, FIRST, LAST, CLEAN, GAIN, PLUGIN_PORT_COUNT};

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
    float *out_1;
    float *step;
    float *first;
    float *last;
    float *clean;
    float *gain;
    
    int nBuffers;
    int Qcolumn;
    int hopa;
    int N;
    int cont;
    
    double s;
    float g;
    
    int *Hops;
    double *frames;
    double *ysaida;
    double *ysaida2;
    double *yshift;
    double **b;
    
	float *frames2;
	float *q;
    fftwf_complex *fXa;
    fftwf_complex *fXs;
    
    cx_vec Xa;
    cx_vec Xs;
    cx_vec XaPrevious;
    vec Xa_arg;
    vec XaPrevious_arg;
    vec Phi;
    vec PhiPrevious;
    vec d_phi;
    vec d_phi_prime;
    vec d_phi_wrapped;
    vec omega_true_sobre_fs;
    vec AUX;
    vec Xa_abs;
    vec I;
    vec w;
        
    fftwf_plan p;
	fftwf_plan p2;
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
    
    plugin->nBuffers = 34;
    plugin->Qcolumn = plugin->nBuffers;
    plugin->hopa = TAMANHO_DO_BUFFER;
    plugin->N = plugin->nBuffers*plugin->hopa;
    plugin->cont = 0;
    
    plugin->s = 0;
    plugin->g = 1;
    
    plugin->Hops = (int*)calloc(plugin->Qcolumn,sizeof(int));
    for (int i=0; i<plugin->Qcolumn; i++)
		plugin->Hops[i] = plugin->hopa;
    
    plugin->frames = (double*)calloc(plugin->N,sizeof(double));
    plugin->ysaida = (double*)calloc(2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa),sizeof(double));
    plugin->yshift = (double*)calloc(plugin->hopa,sizeof(double));
    plugin->b = (double**)calloc(plugin->hopa,sizeof(double*));
    
    plugin->frames2 = fftwf_alloc_real(plugin->N);
    plugin->q = fftwf_alloc_real(plugin->N);
    plugin->fXa = fftwf_alloc_complex(plugin->N/2 + 1);
	plugin->fXs = fftwf_alloc_complex(plugin->N/2 + 1);
    
    plugin->Xa.zeros(plugin->N/2 + 1); 
	plugin->Xs.zeros(plugin->N/2 + 1); 
	plugin->XaPrevious.zeros(plugin->N/2 + 1);
	plugin->Xa_arg.zeros(plugin->N/2 + 1);
	plugin->XaPrevious_arg.zeros(plugin->N/2 + 1);
	plugin->Phi.zeros(plugin->N/2 + 1);
	plugin->PhiPrevious.zeros(plugin->N/2 + 1);
    plugin->d_phi.zeros(plugin->N/2 + 1);
	plugin->d_phi_prime.zeros(plugin->N/2 + 1);
	plugin->d_phi_wrapped.zeros(plugin->N/2 + 1);
	plugin->omega_true_sobre_fs.zeros(plugin->N/2 + 1);
	plugin->AUX.zeros(plugin->N/2 + 1);
	plugin->Xa_abs.zeros(plugin->N/2 + 1);
	plugin->w.zeros(plugin->N); hann(plugin->N,&plugin->w);
	plugin->I.zeros(plugin->N/2 + 1); plugin->I = linspace(0, plugin->N/2,plugin->N/2 + 1);
    
    for (int i=0; i<(plugin->nBuffers); i++)
		plugin->b[i] = &plugin->frames[i*plugin->hopa];
	
	plugin->p = fftwf_plan_dft_r2c_1d(plugin->N, plugin->frames2, plugin->fXa, FFTW_ESTIMATE);
	plugin->p2 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs, plugin->q, FFTW_ESTIMATE);
	
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
        case FIRST:
            plugin->first = (float*) data;
            break;
        case LAST:
            plugin->last = (float*) data;
            break;
        case CLEAN:
            plugin->clean = (float*) data;
            break;
        case GAIN:
            plugin->gain = (float*) data;
            break;
    }
}

/**********************************************************************************************************************************************************/

void PitchShifter::run(LV2_Handle instance, uint32_t n_samples)
{
    PitchShifter *plugin;
    plugin = (PitchShifter *) instance;
    
    if ( ((plugin->hopa) != (int)n_samples) )
    {
		
		switch ((int)n_samples)
		{
			case 64:
				plugin->nBuffers = 34;
				break;
			case 128:
				plugin->nBuffers = 18;
				break;
			case 256:
				plugin->nBuffers = 10;
				break;
			case 512:
				plugin->nBuffers = 8;
				break;
		}
		
		plugin->Qcolumn = plugin->nBuffers;
		
		plugin->hopa = n_samples;
		plugin->N = plugin->nBuffers*plugin->hopa;
		
		plugin->Hops = (int*)realloc(plugin->Hops,plugin->Qcolumn*sizeof(int));
		for (int i=0; i<plugin->Qcolumn; i++)
			plugin->Hops[i] = plugin->hopa;
		
		plugin->frames = (double*)realloc(plugin->frames,plugin->N*sizeof(double));                                          memset(plugin->frames, 0, plugin->N*sizeof(double) );
		plugin->ysaida = (double*)realloc(plugin->ysaida,2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa)*sizeof(double)); memset(plugin->ysaida, 0, 2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa)*sizeof(double) );
		plugin->yshift = (double*)realloc(plugin->yshift,plugin->hopa*sizeof(double));                                       memset(plugin->yshift, 0, plugin->hopa*sizeof(double) );
		plugin->b = (double**)realloc(plugin->b,plugin->hopa*sizeof(double*));
		
		fftwf_free(plugin->frames2); plugin->frames2 = fftwf_alloc_real(plugin->N);
		fftwf_free(plugin->q);       plugin->q = fftwf_alloc_real(plugin->N);
		fftwf_free(plugin->fXa);     plugin->fXa = fftwf_alloc_complex(plugin->N/2 + 1);
		fftwf_free(plugin->fXs);     plugin->fXs = fftwf_alloc_complex(plugin->N/2 + 1);
		
		plugin->Xa.zeros(plugin->N/2 + 1); 
		plugin->Xs.zeros(plugin->N/2 + 1); 
		plugin->XaPrevious.zeros(plugin->N/2 + 1);
		plugin->Xa_arg.zeros(plugin->N/2 + 1);
		plugin->XaPrevious_arg.zeros(plugin->N/2 + 1);
		plugin->Phi.zeros(plugin->N/2 + 1);
		plugin->PhiPrevious.zeros(plugin->N/2 + 1);
		plugin->d_phi.zeros(plugin->N/2 + 1);
		plugin->d_phi_prime.zeros(plugin->N/2 + 1);
		plugin->d_phi_wrapped.zeros(plugin->N/2 + 1);
		plugin->omega_true_sobre_fs.zeros(plugin->N/2 + 1);
		plugin->AUX.zeros(plugin->N/2 + 1);
		plugin->Xa_abs.zeros(plugin->N/2 + 1);
		plugin->w.zeros(plugin->N); hann(plugin->N,&plugin->w);
		plugin->I.zeros(plugin->N/2 + 1); plugin->I = linspace(0, plugin->N/2,plugin->N/2 + 1);		
		
		for (int i=0; i<plugin->nBuffers; i++)
			plugin->b[i] = &plugin->frames[i*plugin->hopa];
		
		fftwf_destroy_plan(plugin->p);  plugin->p = fftwf_plan_dft_r2c_1d(plugin->N, plugin->frames2, plugin->fXa, FFTW_ESTIMATE);
		fftwf_destroy_plan(plugin->p2); plugin->p2 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs, plugin->q, FFTW_ESTIMATE);
		
		return;
	}

    float media = 0;
    
    for (uint32_t i=0; i<n_samples; i++)
		media += abs(plugin->in[i]);
	
	if (media == 0)
	{
		for (uint32_t i=0; i<n_samples; i++)
			plugin->out_1[i] = 0;
	}
	else
	{
		int hops;
		
		double g_before = plugin->g;
		plugin->g = pow(10, (float)(*(plugin->gain))/20.0);

		plugin->s = (double)(*(plugin->step));
		
		float a;
		float b;
		float c;
		
		a = (float)(*(plugin->first));
		b = (float)(*(plugin->last));
		c = (float)(*(plugin->clean));
		
		float s_;
		
		s_ = a + plugin->s*(b-a);
		hops = round(plugin->hopa*(pow(2,(s_/12))));
		
		for (int k=0; k<plugin->Qcolumn-1; k++)
			plugin->Hops[k] = plugin->Hops[k+1];
		
		plugin->Hops[plugin->Qcolumn-1] = hops;
    
		for (int i=0; i<plugin->hopa; i++)
		{
			for (int j=0; j<(plugin->nBuffers-1); j++)
				plugin->b[j][i] = plugin->b[j+1][i];

			plugin->b[plugin->nBuffers-1][i] = plugin->in[i];
		}
		
		if ( plugin->cont < plugin->nBuffers-1)
		{
			plugin->cont++;
		}
		else
		{
			shift(plugin->N, plugin->hopa, plugin->Hops, plugin->frames, plugin->frames2, &plugin->w, &plugin->XaPrevious, &plugin->Xa_arg, &plugin->Xa_abs, &plugin->XaPrevious_arg, &plugin->PhiPrevious, plugin->yshift, &plugin->Xa, &plugin->Xs, plugin->q, &plugin->Phi, plugin->ysaida, plugin->ysaida2,  plugin->Qcolumn, &plugin->d_phi, &plugin->d_phi_prime, &plugin->d_phi_wrapped, &plugin->omega_true_sobre_fs, &plugin->I, &plugin->AUX, plugin->p, plugin->p2, plugin->fXa, plugin->fXs);

			if (c == 1)
			{
				for (int i=0; i<plugin->hopa; i++)
					plugin->out_1[i] = (g_before + i*((plugin->g - g_before)/(plugin->hopa - 1))) * (float)plugin->yshift[i] + (float)plugin->frames[i];
			}
			else
			{
				for (int i=0; i<plugin->hopa; i++)
					plugin->out_1[i] = (g_before + i*((plugin->g - g_before)/(plugin->hopa - 1))) * (float)plugin->yshift[i];
			}
		}
	}
}

/**********************************************************************************************************************************************************/

void PitchShifter::cleanup(LV2_Handle instance)
{
	PitchShifter *plugin;
	plugin = (PitchShifter *) instance;
	
	free(plugin->Hops);
	free(plugin->frames);
	free(plugin->ysaida);
	free(plugin->yshift);
	free(plugin->b);
	
	fftwf_free(plugin->frames2);
	fftwf_free(plugin->q);
	fftwf_free(plugin->fXa);
	fftwf_free(plugin->fXs);
	
	plugin->Xa.clear();
	plugin->Xs.clear();
	plugin->XaPrevious.clear();
	plugin->Xa_arg.clear();
	plugin->XaPrevious_arg.clear();
	plugin->Phi.clear();
	plugin->PhiPrevious.clear();
    plugin->d_phi.clear();
	plugin->d_phi_prime.clear();
	plugin->d_phi_wrapped.clear();
	plugin->omega_true_sobre_fs.clear();
	plugin->AUX.clear();
	plugin->Xa_abs.clear();
	plugin->w.clear();
	plugin->I.clear();
	
	fftwf_destroy_plan(plugin->p);
	fftwf_destroy_plan(plugin->p2);
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
