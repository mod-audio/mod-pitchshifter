#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lv2.h>
#include <complex>
#include "PitchDetection.h"
#include "shift.h"
#include "window.h"
#include "angle.h"
#include <fftw3.h>

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://portalmod.com/plugins/mod-devel/Harmonizer"
#define BUFFER_SIZE 64
enum {IN, OUT_1, OUT_2, TONE, SCALE, INTERVAL, MODE, LOWNOTE, GAIN_1, GAIN_2, PLUGIN_PORT_COUNT};

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
    float *out_2;
    float *tone;
    float *scale;
    float *interval;
    float *mode;
    float *lownote;
    float *gain_1;
    float *gain_2;
    
    int nBuffers;
    int nBuffers2;
    int Qcolumn;
    int hopa;
    int N;
    int N2;
    int cont;
    
    double s;
    float g1;
    float g2;
    
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
    
    float *frames3;
    float *q2;
    fftwf_complex *fXa2;
    fftwf_complex *fXs2;
    cx_vec Xa2;
    cx_vec Xs2;
        
    vec R;
	vec NORM;
	vec F;
	vec AUTO;
	
	int note;
	int oitava;
	
	double SampleRate;
        
    fftwf_plan p;
	fftwf_plan p2;
	fftwf_plan p3;
	fftwf_plan p4;
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
    plugin->Qcolumn = 1*plugin->nBuffers;
    plugin->hopa = BUFFER_SIZE;
    plugin->N = plugin->nBuffers*plugin->hopa;
    plugin->N2 = plugin->nBuffers2*plugin->hopa;
    plugin->cont = 0;
    
    plugin->s = 0;
    plugin->g1 = 1;
    plugin->g2 = 1;  
    
    plugin->Hops = (int*)calloc(plugin->Qcolumn,sizeof(int));
    for (int i=1;i<=plugin->Qcolumn;i++) plugin->Hops[i-1] = plugin->hopa;
        
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
	plugin->Xa2.zeros(plugin->N + 1);
	plugin->Xs2.zeros(plugin->N + 1);
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
	
	plugin->frames3 = fftwf_alloc_real(2*plugin->N2); memset(plugin->frames3, 0, 2*plugin->N2 );
	plugin->q2 = fftwf_alloc_real(2*plugin->N2);	
	plugin->fXa2 = fftwf_alloc_complex(plugin->N2 + 1);
	plugin->fXs2 = fftwf_alloc_complex(plugin->N2 + 1);
	
	plugin->R.zeros(plugin->N2);
	plugin->NORM.zeros(plugin->N2);
	plugin->F.zeros(plugin->N2);
	plugin->AUTO.zeros(plugin->N2);
    
    for (int i=1 ; i<= (plugin->nBuffers); i++)
    {
		plugin->b[i-1] = &plugin->frames[(i-1)*plugin->hopa];
	}
	
	plugin->p = fftwf_plan_dft_r2c_1d(plugin->N, plugin->frames2, plugin->fXa, FFTW_ESTIMATE);
	plugin->p2 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs, plugin->q, FFTW_ESTIMATE);
	plugin->p3 = fftwf_plan_dft_r2c_1d(2*plugin->N2, plugin->frames3, plugin->fXa2, FFTW_ESTIMATE);
	plugin->p4 = fftwf_plan_dft_c2r_1d(2*plugin->N2, plugin->fXs2, plugin->q2, FFTW_ESTIMATE);
	
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

void PitchShifter::run(LV2_Handle instance, uint32_t n_samples)
{
    PitchShifter *plugin;
    plugin = (PitchShifter *) instance;
    
    if ( ((plugin->hopa) != (int)n_samples) )
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
		
		plugin->Qcolumn = plugin->nBuffers;
		
		plugin->hopa = n_samples;
		plugin->N = plugin->nBuffers*plugin->hopa;
		plugin->N2 = plugin->nBuffers2*plugin->hopa;
		
		plugin->Hops = (int*)realloc(plugin->Hops,plugin->Qcolumn*sizeof(int));
		for (int i=1;i<=plugin->Qcolumn;i++) plugin->Hops[i-1] = plugin->hopa;
		
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
		
		fftwf_free(plugin->frames3); plugin->frames3 = fftwf_alloc_real(2*plugin->N2); memset(plugin->frames3, 0, 2*plugin->N2 );
		fftwf_free(plugin->q2);      plugin->q2 = fftwf_alloc_real(2*plugin->N2);
		fftwf_free(plugin->fXa2);    plugin->fXa2 = fftwf_alloc_complex(plugin->N2 + 1);
		fftwf_free(plugin->fXs2);    plugin->fXs2 = fftwf_alloc_complex(plugin->N2 + 1);		
		
		plugin->R.zeros(plugin->N2);
		plugin->NORM.zeros(plugin->N2);
		plugin->F.zeros(plugin->N2);
		plugin->AUTO.zeros(plugin->N2);	
		
		for (int i=1 ; i<= plugin->nBuffers; i++)
		{
			plugin->b[i-1] = &plugin->frames[(i-1)*plugin->hopa];
		}
		
		fftwf_destroy_plan(plugin->p);  plugin->p = fftwf_plan_dft_r2c_1d(plugin->N, plugin->frames2, plugin->fXa, FFTW_ESTIMATE);
		fftwf_destroy_plan(plugin->p2); plugin->p2 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs, plugin->q, FFTW_ESTIMATE);
		fftwf_destroy_plan(plugin->p3); plugin->p3 = fftwf_plan_dft_r2c_1d(2*plugin->N2, plugin->frames3, plugin->fXa2, FFTW_ESTIMATE);
		fftwf_destroy_plan(plugin->p4); plugin->p4 = fftwf_plan_dft_c2r_1d(2*plugin->N2, plugin->fXs2, plugin->q2, FFTW_ESTIMATE);
		
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
	int Interval = (int)(*(plugin->interval));
	int Mode = (int)(*(plugin->mode));
	int LowNote = (int)(*(plugin->lownote));
		    
    double g1_before = plugin->g1;
    double g2_before = plugin->g2;
    plugin->g1 = pow(10, (float)(*(plugin->gain_1))/20.0);
    plugin->g2 = pow(10, (float)(*(plugin->gain_2))/20.0);
    
	for (int k=1; k<= plugin->Qcolumn-1; k++)
    {
		plugin->Hops[k-1] = plugin->Hops[k];
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
			FindNote(plugin->N2, plugin->frames, plugin->frames3, &plugin->Xa2, &plugin->Xs2, plugin->q2, plugin->Qcolumn, plugin->p3, plugin->p4, plugin->fXa2, plugin->fXs2, &plugin->R, &plugin->NORM, &plugin->F, &plugin->AUTO, plugin->SampleRate, &plugin->note, &plugin->oitava );
			FindStep(plugin->note, plugin->oitava, Tone, Scale, Interval, Mode, LowNote, plugin->hopa, plugin->Qcolumn, &plugin->s, plugin->Hops);
			shift(plugin->N, plugin->hopa, plugin->Hops, plugin->frames, plugin->frames2, &plugin->w, &plugin->XaPrevious, &plugin->Xa_arg, &plugin->Xa_abs, &plugin->XaPrevious_arg, &plugin->PhiPrevious, plugin->yshift, &plugin->Xa, &plugin->Xs, plugin->q, &plugin->Phi, plugin->ysaida, plugin->ysaida2,  plugin->Qcolumn, &plugin->d_phi, &plugin->d_phi_prime, &plugin->d_phi_wrapped, &plugin->omega_true_sobre_fs, &plugin->I, &plugin->AUX, plugin->p, plugin->p2, plugin->fXa, plugin->fXs);
			
				for (int i=1; i<=plugin->hopa; i++)
				{
					plugin->out_1[i-1] = (g1_before + ((plugin->g1 - g1_before)/(plugin->hopa - 1))*(i-1) )*(float)plugin->frames[i-1];
					plugin->out_2[i-1] = (g2_before + ((plugin->g2 - g2_before)/(plugin->hopa - 1))*(i-1) )*(float)plugin->yshift[i-1];
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
	
	fftwf_free(plugin->frames3);
	fftwf_free(plugin->q2);
	fftwf_free(plugin->fXa2);
	fftwf_free(plugin->fXs2);
	
	plugin->Xa.clear();
	plugin->Xs.clear();
	plugin->Xa2.clear();
	plugin->Xs2.clear();
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
	
	plugin->R.clear();
	plugin->NORM.clear();
	plugin->F.clear();
	plugin->AUTO.clear();
	
	fftwf_destroy_plan(plugin->p);
	fftwf_destroy_plan(plugin->p2);
	fftwf_destroy_plan(plugin->p3);
	fftwf_destroy_plan(plugin->p4);
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
