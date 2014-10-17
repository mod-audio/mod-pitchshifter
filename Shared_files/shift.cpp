#include <fftw3.h>
#include "shift.h"
#include "Exp.h"
#include "angle.h"
#include <complex>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <armadillo>

using namespace arma;

PSAnalysis::PSAnalysis( uint32_t n_samples, int nBuffers) //Construtor
{
	Qcolumn = nBuffers;
	hopa = n_samples;
	N = nBuffers*n_samples;

	frames = new double[N]; for (int i = 0; i < N; i++) frames[i] = 0;
	w.zeros(N); hann(plugin->N,&w);
	frames2 = fftwf_alloc_real(N); for (int i = 0; i < N; i++) frames2[i] = 0;
	p = fftwf_plan_dft_r2c_1d(N, frames2, fXa, FFTW_ESTIMATE);
	fXa = fftwf_alloc_complex(N/2 + 1);
	Xa.zeros(N/2 + 1);
	Xa_arg.zeros(N/2 + 1);
	Xa_abs.zeros(N/2 + 1);
	XaPrevious.zeros(N/2 + 1);
	d_phi.zeros(N/2 + 1);
	d_phi_prime.zeros(N/2 + 1);
	I.zeros(N/2 + 1); I = linspace(0, N/2, N/2 + 1);
	AUX.zeros(N/2 + 1);
	d_phi_wrapped.zeros(N/2 + 1);
	omega_true_sobre_fs.zeros(N/2 + 1);
}

PSSinthesis::PSSinthesis( PSAnalysis *obj) //Construtor
{
	(this->obj)->obj;

	hops = new int[obj->Qcolumn]; for (int i = 0; i < obj->Qcolumn;i++) hops[i] = obj->hopa;
}

void shift(int N, int hopa, int *hops, double *frames, float *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, vec *PhiPrevious, double *yshift, cx_vec *Xa, cx_vec *Xs, float *q, vec *Phi, double *ysaida, double *ysaida2, int Qcolumn, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftwf_plan p, fftwf_plan p2, fftwf_complex *fXa, fftwf_complex *fXs)
{
	static bool first = true;
	
	//Some declaration
	int L;
	L = N;
	for (int i=1; i<= Qcolumn-1; i++)
	{
		L = L + hops[i-1];
	}
	double r;
	complex<double> j;
	int n1;
	int n2;
	double n3;
	
	//Some inicialization
	
	ysaida2 = &ysaida[L-N];
	
	//Starts now
	
	//Windowing
	
	for (int i=1; i<=N; i++)
	{
		frames2[i-1] = frames[i-1]*w[0](i-1)/(sqrt(((double)N/(2*(double)hopa))));
	}
	
	/*Analysis*/
	fftwf_execute(p);
	
	/*Processing*/
	
	for (int i=1; i<=(N/2 + 1); i++)
	{
		Xa[0](i-1) = cx_float(fXa[i-1][0], fXa[i-1][1]);
		Xa_arg[0](i-1) = angle(Xa[0](i-1));
	}
	d_phi[0] = Xa_arg[0] - XaPrevious_arg[0];
	d_phi_prime[0] = d_phi[0] - ((2*M_PI * hopa) / N) * I[0];
	AUX[0] = floor((d_phi_prime[0] + M_PI) / (2*M_PI));
	d_phi_wrapped[0] = d_phi_prime[0] - AUX[0] * (2*M_PI);
	omega_true_sobre_fs[0] = (2*M_PI/N) * I[0] + d_phi_wrapped[0] / hopa;

	// We can divide the code in two here

	Phi[0] = PhiPrevious[0] + (hops[Qcolumn-1])*omega_true_sobre_fs[0] ;
	Xa_abs[0] = abs(Xa[0]);
	for (int i=1; i<=(N/2 + 1); i++)
	{
        Xs[0](i-1) = ExponencialComplexa(Phi[0](i-1));
	}
	Xs[0] = Xa_abs[0] % Xs[0];
	XaPrevious[0] = Xa[0];
	XaPrevious_arg[0] = Xa_arg[0];
	PhiPrevious[0] = Phi[0];
	
	
	for (int i=1; i<=(N/2 + 1); i++)
	{
        fXs[i-1][0] = real(Xs[0](i-1));
        fXs[i-1][1] = imag(Xs[0](i-1));
	}
	
	/*Synthesis*/
	fftwf_execute(p2);
	
	for (int i=1; i<=N; i++)
	{
		q[i-1] = q[i-1]*w[0](i-1)/(N*sqrt((N/(2*hops[Qcolumn-1]))));
	}
	
	if ( first == true)
	{
		first = false;
		for (int i=1; i<=L-N; i++)
		{
			ysaida[i-1] = 0;
		}
		for (int i=L-N+1; i<=L; i++)
		{
			ysaida[i-1] = q[i-(L-N+1)];
		}
	}
	else
	{
		for (int i=L-N+1; i<=L; i++)
		{
			ysaida[i-1] = ysaida[i-1] + q[i-(L-N+1)];
		}
	}
	//Linear interpolation
	r = ((double)hops[Qcolumn-1])/((double)hopa);

        for (int n=1; n <= hopa; n++)
        {
            n3 = (((double)n-1)*r+1);
            n1 = floor(n3);
            n2 = ceil(n3);
            yshift[n-1] = ysaida2[n1-1] + ((ysaida2[n2-1]-ysaida2[n1-1]))*(n3 - (double)n1);
		}
		
	//Shift ysaida hops[0] left
	for (int i=1; i<=L-hops[0]; i++)
	{
		ysaida[i-1] = ysaida[i-1+hops[0]];
	}
	for (int i=L-hops[0]+1; i<=L; i++)
	{
		ysaida[i-1] = 0;
	}
	
}

void shift1(int N, int hopa, double *frames, float *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, cx_vec *Xa, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftwf_plan p, fftwf_complex *fXa)
{		
	//Starts now
	
	//Windowing
	
	for (int i=1; i<=N; i++)
	{
		frames2[i-1] = frames[i-1]*w[0](i-1)/(sqrt(((double)N/(2*(double)hopa))));
	}
	
	/*Analysis*/
	fftwf_execute(p);
	
	/*Processing*/
	
	for (int i=1; i<=(N/2 + 1); i++)
	{
		Xa[0](i-1) = cx_float(fXa[i-1][0], fXa[i-1][1]);
		Xa_arg[0](i-1) = angle(Xa[0](i-1));
	}
	d_phi[0] = Xa_arg[0] - XaPrevious_arg[0];
	d_phi_prime[0] = d_phi[0] - ((2*M_PI * hopa) / N) * I[0];
	AUX[0] = floor((d_phi_prime[0] + M_PI) / (2*M_PI));
	d_phi_wrapped[0] = d_phi_prime[0] - AUX[0] * (2*M_PI);
	omega_true_sobre_fs[0] = (2*M_PI/N) * I[0] + d_phi_wrapped[0] / hopa;

	Xa_abs[0] = abs(Xa[0]);
	XaPrevious[0] = Xa[0];
	XaPrevious_arg[0] = Xa_arg[0];

}

void shift2(int *hops, vec *PhiPrevious, double *yshift, cx_vec *Xs,  float *q, vec *Phi, double *ysaida, double *ysaida2, int Qcolumn, fftwf_plan p2, fftwf_complex *fXs,int N, int hopa, double *frames, float *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, cx_vec *Xa, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftwf_plan p, fftwf_complex *fXa)
{
	static bool first = true;
	
	//Some declaration
	int L;
	L = N;
	for (int i=1; i<= Qcolumn-1; i++)
	{
		L = L + hops[i-1];
	}
	double r;
	complex<double> j;
	int n1;
	int n2;
	double n3;
	
	//Some inicialization
	
	ysaida2 = &ysaida[L-N];

	// We can divide the code in two here

	Phi[0] = PhiPrevious[0] + (hops[Qcolumn-1])*omega_true_sobre_fs[0] ;
	for (int i=1; i<=(N/2 + 1); i++)
	{
        Xs[0](i-1) = ExponencialComplexa(Phi[0](i-1));
	}
	Xs[0] = Xa_abs[0] % Xs[0];
	PhiPrevious[0] = Phi[0];
	
	
	for (int i=1; i<=(N/2 + 1); i++)
	{
        fXs[i-1][0] = real(Xs[0](i-1));
        fXs[i-1][1] = imag(Xs[0](i-1));
	}
	
	/*Synthesis*/
	fftwf_execute(p2);
	
	for (int i=1; i<=N; i++)
	{
		q[i-1] = q[i-1]*w[0](i-1)/(N*sqrt((N/(2*hops[Qcolumn-1]))));
	}
	
	if ( first == true)
	{
		first = false;
		for (int i=1; i<=L-N; i++)
		{
			ysaida[i-1] = 0;
		}
		for (int i=L-N+1; i<=L; i++)
		{
			ysaida[i-1] = q[i-(L-N+1)];
		}
	}
	else
	{
		for (int i=L-N+1; i<=L; i++)
		{
			ysaida[i-1] = ysaida[i-1] + q[i-(L-N+1)];
		}
	}
	//Linear interpolation
	r = ((double)hops[Qcolumn-1])/((double)hopa);

        for (int n=1; n <= hopa; n++)
        {
            n3 = (((double)n-1)*r+1);
            n1 = floor(n3);
            n2 = ceil(n3);
            yshift[n-1] = ysaida2[n1-1] + ((ysaida2[n2-1]-ysaida2[n1-1]))*(n3 - (double)n1);
		}
		
	//Shift ysaida hops[0] left
	for (int i=1; i<=L-hops[0]; i++)
	{
		ysaida[i-1] = ysaida[i-1+hops[0]];
	}
	for (int i=L-hops[0]+1; i<=L; i++)
	{
		ysaida[i-1] = 0;
	}
	
}

