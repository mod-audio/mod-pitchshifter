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
#define TAMANHO_DO_BUFFER 1024
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
    float *in;
    float *out_1;
    float *step;
    float *first;
    float *last;
    float *clean;
    float *gain;
    
    float g;
    
    float max;
    float min;
    
    int hopa;
    int N;
    int cont;
    int Qcolumn;
    int nBuffers;
    double s;
    
    double *frames;
    double **b;
    double *ysaida;
    double *ysaida2;
    double *yshift;
    int *Hops;
    
    vec w;
    cx_vec frames2;
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
    vec I;
    vec AUX;
    vec Xa_abs;
    vec q;
    cx_vec qaux;
    mat Q;
        
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
    plugin->Qcolumn = 24;
    plugin->nBuffers = 12;
    //Começam os testes
    plugin->hopa = TAMANHO_DO_BUFFER;
    plugin->N = plugin->nBuffers*TAMANHO_DO_BUFFER;
    
    plugin->g = 1;    
    
    plugin->Hops = (int*)malloc(plugin->Qcolumn*sizeof(int));
    plugin->frames = (double*)malloc(plugin->N*sizeof(double));
    plugin->b = (double**)malloc(plugin->hopa*sizeof(double*));
    plugin->ysaida = (double*)malloc((plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa)*sizeof(double));
    plugin->yshift = (double*)malloc(plugin->hopa*sizeof(double));
    
    plugin->w.resize(plugin->N);
    plugin->frames2.resize(plugin->N);
    plugin->Xa.resize(plugin->N);
	plugin->Xs.resize(plugin->N);
	plugin->XaPrevious.resize(plugin->N);
	plugin->Xa_arg.resize(plugin->N);
	plugin->XaPrevious_arg.resize(plugin->N);
	plugin->Phi.resize(plugin->N);
    plugin->PhiPrevious.resize(plugin->N);
    plugin->d_phi.resize(plugin->N);
	plugin->d_phi_prime.resize(plugin->N);
	plugin->d_phi_wrapped.resize(plugin->N);
	plugin->omega_true_sobre_fs.resize(plugin->N);
	plugin->I.resize(plugin->N);
	plugin->AUX.resize(plugin->N);
	plugin->Xa_abs.resize(plugin->N);
	plugin->q.resize(plugin->N);
	plugin->qaux.resize(plugin->N);
	plugin->Q.resize(plugin->N, plugin->Qcolumn);
	
	plugin->I = linspace(0, plugin->N-1,plugin->N);
	
    hann(plugin->N,&plugin->w);
    
    for (int i=1 ; i<= (plugin->nBuffers); i++)
    {
		plugin->b[i-1] = &plugin->frames[(i-1)*plugin->hopa];
	}

    plugin->cont = 0;
    for (int i=1;i<=plugin->N;i++)
    {
		plugin->frames[i-1] = 0;
		plugin->PhiPrevious(i-1) = 0;
		plugin->XaPrevious(i-1) = 0;
		plugin->XaPrevious_arg(i-1) = 0;
		for (int k=1; k<=plugin->Qcolumn; k++)
		{
			plugin->Q(i-1,k-1) = 0;
		}
	}
	
	for (int k=1; k<=plugin->Qcolumn; k++)
	{
		plugin->Hops[k-1] = plugin->hopa;
	}
	
	plugin->p = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->frames2.colptr(0)), reinterpret_cast<fftw_complex*>(plugin->Xa.colptr(0)), FFTW_FORWARD, FFTW_ESTIMATE);
	plugin->p2 = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->Xs.colptr(0)), reinterpret_cast<fftw_complex*>(plugin->qaux.colptr(0)), FFTW_BACKWARD, FFTW_ESTIMATE);
	
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
    /* double *pfOutput; */
    float media = 0;
    for (uint32_t i=1; i<n_samples; i++)
    {
		media = media + abs(plugin->in[i-1]);
	}
	
	if (media == 0)
	{
		for (uint32_t i=1; i<n_samples; i++)
		{
			plugin->out_1[i-1] = 0;
		}
	}
	else
	{
		
    int hops;
    
    double g_before = plugin->g;
    plugin->g = pow(10, (float)(*(plugin->gain))/20.0);

    //float max_before = plugin->max;
    //float min_before = plugin->min;
    plugin->s = (double)(*(plugin->step));
    
    float a;
    float b;
    float c;
    
    a = (float)(*(plugin->first));
    b = (float)(*(plugin->last));
    c = (float)(*(plugin->clean));
    
    if (a > b)
    {
		plugin->max = a;
		plugin->min = b;
	}
	else
	{
		plugin->max = b;
		plugin->min = a;
	}
    
    float s_;
    
    s_ = a + plugin->s*(b-a);
    hops = round(plugin->hopa*(pow(2,(s_/12))));
    
	for (int k=1; k<= plugin->Qcolumn-1; k++)
    {
		plugin->Hops[k-1] = plugin->Hops[k];
	}
    
    plugin->Hops[plugin->Qcolumn-1] = hops;
    
    if ( ((plugin->hopa) != (int)n_samples) )
    {
		plugin->hopa = n_samples;
		plugin->N = plugin->nBuffers*n_samples;
		
		plugin->w.resize(plugin->N);
		plugin->frames2.resize(plugin->N);
		plugin->Xa.resize(plugin->N);
		plugin->Xs.resize(plugin->N);
		plugin->XaPrevious.resize(plugin->N);
		plugin->Xa_arg.resize(plugin->N);
		plugin->XaPrevious_arg.resize(plugin->N);
		plugin->Phi.resize(plugin->N);
		plugin->PhiPrevious.resize(plugin->N);
		plugin->d_phi.resize(plugin->N);
		plugin->d_phi_prime.resize(plugin->N);
		plugin->d_phi_wrapped.resize(plugin->N);
		plugin->omega_true_sobre_fs.resize(plugin->N);
		plugin->I.resize(plugin->N);
		plugin->AUX.resize(plugin->N);
		plugin->Xa_abs.resize(plugin->N);
		plugin->q.resize(plugin->N);
		plugin->qaux.resize(plugin->N);
		plugin->Q.resize(plugin->N, plugin->Qcolumn);
		
		plugin->I = linspace(0, plugin->N-1,plugin->N);
		
		hann(plugin->N,&plugin->w);
		fftw_destroy_plan(plugin->p);
		fftw_destroy_plan(plugin->p2);
		plugin->p = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->frames2.colptr(0)), reinterpret_cast<fftw_complex*>(plugin->Xa.colptr(0)), FFTW_FORWARD, FFTW_ESTIMATE);
		plugin->p2 = fftw_plan_dft_1d(plugin->N, reinterpret_cast<fftw_complex*>(plugin->Xs.colptr(0)), reinterpret_cast<fftw_complex*>(plugin->qaux.colptr(0)), FFTW_BACKWARD, FFTW_ESTIMATE);
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
			shift(plugin->N, plugin->hopa, plugin->Hops, plugin->frames, &plugin->frames2, &plugin->w, &plugin->XaPrevious, &plugin->Xa_arg, &plugin->Xa_abs, &plugin->XaPrevious_arg, &plugin->PhiPrevious, &plugin->Q, plugin->yshift, &plugin->Xa, &plugin->Xs, &plugin->q, &plugin->qaux, &plugin->Phi, plugin->ysaida, plugin->ysaida2,  plugin->Qcolumn, &plugin->d_phi, &plugin->d_phi_prime, &plugin->d_phi_wrapped, &plugin->omega_true_sobre_fs, &plugin->I, &plugin->AUX, plugin->p, plugin->p2);
			if (c == 1)
			{
				for (int i=1; i<=plugin->hopa; i++)
				{
					plugin->out_1[i-1] = (g_before + ((plugin->g - g_before)/(plugin->hopa - 1))*(i-1) )*(float)plugin->yshift[i-1] + (float)plugin->frames[i-1];
				}
			}
			else
			{
				for (int i=1; i<=plugin->hopa; i++)
				{
				plugin->out_1[i-1] = (g_before + ((plugin->g - g_before)/(plugin->hopa - 1))*(i-1) )*(float)plugin->yshift[i-1];
				}
			}
		}
		
	}

}

/**********************************************************************************************************************************************************/

void PitchShifter::cleanup(LV2_Handle instance)
{
	PitchShifter *plugin;
	plugin = (PitchShifter *) instance;
	
	free(plugin->ysaida);
	free(plugin->yshift);
	free(plugin->Hops);
	free(plugin->frames);
	free(plugin->b);
	
	fftw_destroy_plan(plugin->p);
	fftw_destroy_plan(plugin->p2);
	
	plugin->Q.clear();
	plugin->Xa.clear();
	plugin->Xa_arg.clear();
	plugin->Xa_abs.clear();
	plugin->Xs.clear();
	plugin->q.clear();
	plugin->qaux.clear();
	plugin->Phi.clear();
	plugin->frames2.clear();
	plugin->w.clear();
	plugin->PhiPrevious.clear();
	plugin->XaPrevious.clear();
	plugin->XaPrevious_arg.clear();
	plugin->d_phi.clear();
	plugin->d_phi_prime.clear();
	plugin->d_phi_wrapped.clear();
	plugin->I.clear();
	plugin->omega_true_sobre_fs.clear();
	plugin->AUX.clear();	
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
