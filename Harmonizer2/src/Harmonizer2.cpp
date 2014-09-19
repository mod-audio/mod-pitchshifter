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

    int nBuffers;
    int Qcolumn;
    int hopa;
    int N;
    int cont;

    double *frames;
    float *frames2;
    double **b;
    fftwf_complex *fXa;
    cx_vec Xa;
    cx_vec XaPrevious;
    vec Xa_arg;
    vec XaPrevious_arg;
     vec d_phi;
    vec d_phi_prime;
    vec d_phi_wrapped;
    vec omega_true_sobre_fs;
    vec AUX;
    vec Xa_abs;
    vec I;
    vec w;
    fftwf_plan p;

    float g_clean;

	float g_1;    
    double s_1;
    int *Hops_1;
    double *ysaida_1;
    double *ysaida2_1;
    double *yshift_1;
    float *q_1;
    fftwf_complex *fXs_1;
    cx_vec Xs_1;
    vec Phi_1;
    vec PhiPrevious_1;
    fftwf_plan p2_1;

    float g_2;
    double s_2;
    int *Hops_2;
    double *ysaida_2;
    double *ysaida2_2;
    double *yshift_2;
    float *q_2;
    fftwf_complex *fXs_2;
    cx_vec Xs_2;
    vec Phi_2;
    vec PhiPrevious_2;
    fftwf_plan p2_2;

    /*************/

    int nBuffers2;
    int N2;
    float *frames3;
    fftwf_plan p3;
	fftwf_plan p4;
    
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
    plugin->Qcolumn = 1*plugin->nBuffers;
    plugin->hopa = TAMANHO_DO_BUFFER;
    plugin->N = plugin->nBuffers*plugin->hopa;
    plugin->cont = 0;

    plugin->s_1 = 0;
    plugin->s_2 = 0;
    plugin->g_clean = 1;
    plugin->g_1 = 1;
    plugin->g_2 = 1;

    plugin->frames = (double*)calloc(plugin->N,sizeof(double));
    plugin->b = (double**)calloc(plugin->hopa,sizeof(double*));

    plugin->frames2 = fftwf_alloc_real(plugin->N);
    plugin->fXa = fftwf_alloc_complex(plugin->N/2 + 1);

    plugin->Xa.zeros(plugin->N/2 + 1);
    plugin->XaPrevious.zeros(plugin->N/2 + 1);
    plugin->Xa_arg.zeros(plugin->N/2 + 1);
	plugin->XaPrevious_arg.zeros(plugin->N/2 + 1);
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
    
    plugin->Hops_1 = (int*)calloc(plugin->Qcolumn,sizeof(int));
    for (int i=0; i<plugin->Qcolumn;i++)
		plugin->Hops_1[i] = plugin->hopa;

    plugin->ysaida_1 = (double*)calloc(2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa),sizeof(double));
    plugin->yshift_1 = (double*)calloc(plugin->hopa,sizeof(double));
    plugin->q_1 = fftwf_alloc_real(plugin->N);
	plugin->fXs_1 = fftwf_alloc_complex(plugin->N/2 + 1);
	plugin->Xs_1.zeros(plugin->N/2 + 1);
	plugin->Phi_1.zeros(plugin->N/2 + 1);
	plugin->PhiPrevious_1.zeros(plugin->N/2 + 1);
	plugin->p2_1 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs_1, plugin->q_1, FFTW_ESTIMATE);

	plugin->Hops_2 = (int*)calloc(plugin->Qcolumn,sizeof(int));
    for (int i=0; i<plugin->Qcolumn; i++)
		plugin->Hops_2[i] = plugin->hopa;

    plugin->ysaida_2 = (double*)calloc(2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa),sizeof(double));
    plugin->yshift_2 = (double*)calloc(plugin->hopa,sizeof(double));
    plugin->q_2 = fftwf_alloc_real(plugin->N);
	plugin->fXs_2 = fftwf_alloc_complex(plugin->N/2 + 1);
	plugin->Xs_2.zeros(plugin->N/2 + 1);
	plugin->Phi_2.zeros(plugin->N/2 + 1);
	plugin->PhiPrevious_2.zeros(plugin->N/2 + 1);
	plugin->p2_2 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs_2, plugin->q_2, FFTW_ESTIMATE);

	plugin->nBuffers2 = 16;
    plugin->N2 = plugin->nBuffers2*plugin->hopa;   

	plugin->Xa2.zeros(plugin->N + 1);
	plugin->Xs2.zeros(plugin->N + 1);
	
	plugin->frames3 = fftwf_alloc_real(2*plugin->N2); memset(plugin->frames3, 0, 2*plugin->N2 );
	plugin->q2 = fftwf_alloc_real(2*plugin->N2);	
	plugin->fXa2 = fftwf_alloc_complex(plugin->N2 + 1);
	plugin->fXs2 = fftwf_alloc_complex(plugin->N2 + 1);
	
	plugin->R.zeros(plugin->N2);
	plugin->NORM.zeros(plugin->N2);
	plugin->F.zeros(plugin->N2);
	plugin->AUTO.zeros(plugin->N2);

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

		free(plugin->frames); plugin->frames = (double*)calloc(plugin->N,sizeof(double));
    	free(plugin->b);      plugin->b = (double**)calloc(plugin->hopa,sizeof(double*));

    	fftwf_free(plugin->frames2); plugin->frames2 = fftwf_alloc_real(plugin->N);
    	fftwf_free(plugin->fXa);     plugin->fXa = fftwf_alloc_complex(plugin->N/2 + 1);

    	plugin->Xa.zeros(plugin->N/2 + 1);
    	plugin->XaPrevious.zeros(plugin->N/2 + 1);
    	plugin->Xa_arg.zeros(plugin->N/2 + 1);
		plugin->XaPrevious_arg.zeros(plugin->N/2 + 1);
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

		free(plugin->Hops_1);      plugin->Hops_1 = (int*)calloc(plugin->Qcolumn,sizeof(int)); for (int i=1;i<=plugin->Qcolumn;i++) plugin->Hops_1[i-1] = plugin->hopa;
		free(plugin->ysaida_1);    plugin->ysaida_1 = (double*)calloc(2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa),sizeof(double));
	    free(plugin->yshift_1);    plugin->yshift_1 = (double*)calloc(plugin->hopa,sizeof(double));
	    fftwf_free(plugin->q_1);   plugin->q_1 = fftwf_alloc_real(plugin->N);
	    fftwf_free(plugin->fXs_1); plugin->fXs_1 = fftwf_alloc_complex(plugin->N/2 + 1);
	    plugin->Xs_1.zeros(plugin->N/2 + 1);
	    plugin->Phi_1.zeros(plugin->N/2 + 1);
		plugin->PhiPrevious_1.zeros(plugin->N/2 + 1);
		fftwf_destroy_plan(plugin->p2_1); plugin->p2_1 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs_1, plugin->q_1, FFTW_ESTIMATE);

		free(plugin->Hops_2);   plugin->Hops_2 = (int*)calloc(plugin->Qcolumn,sizeof(int)); for (int i=1;i<=plugin->Qcolumn;i++) plugin->Hops_2[i-1] = plugin->hopa;
		free(plugin->ysaida_2); plugin->ysaida_2 = (double*)calloc(2*(plugin->N + 2*(plugin->Qcolumn-1)*plugin->hopa),sizeof(double));
	    free(plugin->yshift_2); plugin->yshift_2 = (double*)calloc(plugin->hopa,sizeof(double));
	    free(plugin->q_2);      plugin->q_2 = fftwf_alloc_real(plugin->N);
	    free(plugin->fXs_2);    plugin->fXs_2 = fftwf_alloc_complex(plugin->N/2 + 1);
	    plugin->Xs_2.zeros(plugin->N/2 + 1);
	    plugin->Phi_2.zeros(plugin->N/2 + 1);
		plugin->PhiPrevious_2.zeros(plugin->N/2 + 1);
		fftwf_destroy_plan(plugin->p2_2); plugin->p2_2 = fftwf_plan_dft_c2r_1d(plugin->N, plugin->fXs_2, plugin->q_2, FFTW_ESTIMATE);

		/*************/
		
		fftwf_free(plugin->frames3); plugin->frames3 = fftwf_alloc_real(2*plugin->N2); memset(plugin->frames3, 0, 2*plugin->N2 );
		fftwf_free(plugin->q2);      plugin->q2 = fftwf_alloc_real(2*plugin->N2);
		fftwf_free(plugin->fXa2);    plugin->fXa2 = fftwf_alloc_complex(plugin->N2 + 1);
		fftwf_free(plugin->fXs2);    plugin->fXs2 = fftwf_alloc_complex(plugin->N2 + 1);		
		
		plugin->R.zeros(plugin->N2);
		plugin->NORM.zeros(plugin->N2);
		plugin->F.zeros(plugin->N2);
		plugin->AUTO.zeros(plugin->N2);	
		
		fftwf_destroy_plan(plugin->p3); plugin->p3 = fftwf_plan_dft_r2c_1d(2*plugin->N2, plugin->frames3, plugin->fXa2, FFTW_ESTIMATE);
		fftwf_destroy_plan(plugin->p4); plugin->p4 = fftwf_plan_dft_c2r_1d(2*plugin->N2, plugin->fXs2, plugin->q2, FFTW_ESTIMATE);
		
		return;
	}

    float media = 0;
    
    for (uint32_t i=0; i<n_samples; i++)
		media += abs(plugin->in[i]);
	
	if (media == 0)
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
		int Interval_1 = (int)(*(plugin->interval_1));
		int Interval_2 = (int)(*(plugin->interval_2));
		int Mode = (int)(*(plugin->mode));
		int LowNote = (int)(*(plugin->lownote));
	
		double g0_before = plugin->g_clean;	    
		double g1_before = plugin->g_1;
		double g2_before = plugin->g_2;
		plugin->g_clean = pow(10, (float)(*(plugin->gain_clean))/20.0);
		plugin->g_1 = pow(10, (float)(*(plugin->gain_1))/20.0);
		plugin->g_2 = pow(10, (float)(*(plugin->gain_2))/20.0);
		
		for (int k=0; k<plugin->Qcolumn-1; k++)
			plugin->Hops_1[k] = plugin->Hops_1[k+1];
		for (int k=0; k<plugin->Qcolumn-1; k++)
			plugin->Hops_2[k] = plugin->Hops_2[k+1];

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
			FindNote(plugin->N2, plugin->frames, plugin->frames3, &plugin->Xa2, &plugin->Xs2, plugin->q2, plugin->Qcolumn, plugin->p3, plugin->p4, plugin->fXa2, plugin->fXs2, &plugin->R, &plugin->NORM, &plugin->F, &plugin->AUTO, plugin->SampleRate, &plugin->note, &plugin->oitava );

			FindStep(plugin->note, plugin->oitava, Tone, Scale, Interval_1, Mode, LowNote, plugin->hopa, plugin->Qcolumn, &plugin->s_1, plugin->Hops_1);

			FindStep(plugin->note, plugin->oitava, Tone, Scale, Interval_2, Mode, LowNote, plugin->hopa, plugin->Qcolumn, &plugin->s_2, plugin->Hops_2);

			shift1(plugin->N, plugin->hopa, plugin->frames, plugin->frames2, &plugin->w, &plugin->XaPrevious, &plugin->Xa_arg, &plugin->Xa_abs, &plugin->XaPrevious_arg, &plugin->Xa, &plugin->d_phi, &plugin->d_phi_prime, &plugin->d_phi_wrapped, &plugin->omega_true_sobre_fs, &plugin->I, &plugin->AUX, plugin->p, plugin->fXa);

			shift2(plugin->Hops_1, &plugin->PhiPrevious_1, plugin->yshift_1, &plugin->Xs_1, plugin->q_1, &plugin->Phi_1, plugin->ysaida_1, plugin->ysaida2_1, plugin->Qcolumn, plugin->p2_1, plugin->fXs_1,plugin->N, plugin->hopa, plugin->frames, plugin->frames2, &plugin->w, &plugin->XaPrevious, &plugin->Xa_arg, &plugin->Xa_abs, &plugin->XaPrevious_arg, &plugin->Xa, &plugin->d_phi, &plugin->d_phi_prime, &plugin->d_phi_wrapped, &plugin->omega_true_sobre_fs, &plugin->I, &plugin->AUX, plugin->p, plugin->fXa);

			shift2(plugin->Hops_2, &plugin->PhiPrevious_2, plugin->yshift_2, &plugin->Xs_2, plugin->q_2, &plugin->Phi_2, plugin->ysaida_2, plugin->ysaida2_2, plugin->Qcolumn, plugin->p2_2, plugin->fXs_2,plugin->N, plugin->hopa, plugin->frames, plugin->frames2, &plugin->w, &plugin->XaPrevious, &plugin->Xa_arg, &plugin->Xa_abs, &plugin->XaPrevious_arg, &plugin->Xa, &plugin->d_phi, &plugin->d_phi_prime, &plugin->d_phi_wrapped, &plugin->omega_true_sobre_fs, &plugin->I, &plugin->AUX, plugin->p, plugin->fXa);
			
			for (int i=0; i<plugin->hopa; i++)
			{
				plugin->out_clean[i] = (g0_before + i*((plugin->g_clean - g0_before)/(plugin->hopa - 1))) * (float)plugin->frames[i];
				plugin->out_1[i] = (g1_before + i*((plugin->g_1 - g1_before)/(plugin->hopa - 1))) * (float)plugin->yshift_1[i];
				plugin->out_2[i] = (g2_before + i*((plugin->g_2 - g2_before)/(plugin->hopa - 1))) * (float)plugin->yshift_2[i];
			}
		}
	}
}

/**********************************************************************************************************************************************************/

void PitchShifter::cleanup(LV2_Handle instance)
{
	PitchShifter *plugin;
	plugin = (PitchShifter *) instance;
	
	free(plugin->frames);
	free(plugin->b);
	
	fftwf_free(plugin->frames2);
	fftwf_free(plugin->fXa);
	
	plugin->Xa.clear();
	plugin->XaPrevious.clear();
	plugin->Xa_arg.clear();
	plugin->XaPrevious_arg.clear();
    plugin->d_phi.clear();
	plugin->d_phi_prime.clear();
	plugin->d_phi_wrapped.clear();
	plugin->omega_true_sobre_fs.clear();
	plugin->AUX.clear();
	plugin->Xa_abs.clear();
	plugin->w.clear();
	plugin->I.clear();
	
	fftwf_destroy_plan(plugin->p);

	free(plugin->Hops_1);
	free(plugin->ysaida_1);
	free(plugin->yshift_1);
	fftwf_free(plugin->q_1);
	fftwf_free(plugin->fXs_1);
	plugin->Xs_1.clear();
	plugin->Phi_1.clear();
	plugin->PhiPrevious_1.clear();
	fftwf_destroy_plan(plugin->p2_1);

	free(plugin->Hops_2);
	free(plugin->ysaida_2);
	free(plugin->yshift_2);
	fftwf_free(plugin->q_2);
	fftwf_free(plugin->fXs_2);
	plugin->Xs_2.clear();
	plugin->Phi_2.clear();
	plugin->PhiPrevious_2.clear();
	fftwf_destroy_plan(plugin->p2_2);

	fftwf_free(plugin->frames3);
	fftwf_free(plugin->q2);
	fftwf_free(plugin->fXa2);
	fftwf_free(plugin->fXs2);
	
	plugin->Xa2.clear();
	plugin->Xs2.clear();

	plugin->R.clear();
	plugin->NORM.clear();
	plugin->F.clear();
	plugin->AUTO.clear();
	
	fftwf_destroy_plan(plugin->p3);
	fftwf_destroy_plan(plugin->p4);
	
    delete ((PitchShifter *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchShifter::extension_data(const char* uri)
{
    return NULL;
}
