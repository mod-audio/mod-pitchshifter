#include <fftw3.h>
#include "shift.h"
#include <complex>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>


void shift(int N, int hopa, int hops, double *frames, double *w, complex<double> *XaPrevious, double *PhiPrevious, double **Q, double *yshift, complex<double> *Xa, complex<double> *Xs, double *q, complex<double> *qaux, complex<double> *framesaux, double *Phi, double *ysaida, double *ysaida2, int Qcolumn)
{
	//Some declaration
	double AUX;
	double d_phi;
	double d_phi_prime;
	double d_phi_wrapped;
	double omega_true_sobre_fs;
	int L = N + (Qcolumn-1)*hops;
	double r;
	complex<double> j;
	int n1;
	int n2;
	double n3;
	
	fftw_plan p;
	fftw_plan p2;
	
	//Some inicialization
	
	p = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(framesaux), reinterpret_cast<fftw_complex*>(Xa), FFTW_FORWARD, FFTW_ESTIMATE);
	p2 = fftw_plan_dft_1d(N, reinterpret_cast<fftw_complex*>(Xs), reinterpret_cast<fftw_complex*>(qaux), FFTW_BACKWARD, FFTW_ESTIMATE);
	j = complex<double>(0.0, 1.0);
	for (int i=1; i<=L; i++)
	{
		ysaida[i-1] = 0;
	}
	
	ysaida2 = &ysaida[(Qcolumn-1)*hops];
	
	//Starts now
	
	//Windowing
	for (int i=1; i<=N; i++)
	{
        framesaux[i-1] = (frames[i-1]*w[i-1])/(sqrt(((double)N/(2*(double)hopa))));
	}
	
	/*Analysis*/
	fftw_execute(p);
	
	/*Processing*/
	for (int i=1; i<=N; i++)
	{
		d_phi = arg(Xa[i-1]) - arg(XaPrevious[i-1]);
        d_phi_prime = d_phi - ((2*M_PI*((double)hopa))/((double)N))*((double)i-1);
        AUX = ((d_phi_prime +M_PI)/(2*M_PI));
        AUX = floor(AUX);
        d_phi_wrapped = d_phi_prime-AUX*(2*M_PI);
        omega_true_sobre_fs = (2*M_PI/((double)N))*((double)i-1) + d_phi_wrapped/((double)hopa);
        Phi[i-1] = PhiPrevious[i-1] + ((double)hops)*omega_true_sobre_fs;
        Xs[i-1] = ((complex<double>)abs(Xa[i-1]))*exp(j*((complex<double>)Phi[i-1]));
        XaPrevious[i-1] = Xa[i-1];
        PhiPrevious[i-1] = Phi[i-1];
	}
	
	/*Synthesis*/
	fftw_execute(p2);
	for (int i=1; i<=N; i++)
	{
		q[i-1] = real(qaux[i-1])/((double)N);
		q[i-1] = q[i-1]*w[i-1]/(sqrt(((double)N/(2*(double)hops))));
		for (int j=1; j<=(Qcolumn-1); j++)
		{
			Q[i-1][j-1] = Q[i-1][j];
		}
        Q[i-1][Qcolumn-1] = q[i-1];
	}
	
	for (int k=1; k <= Qcolumn; k++)
	{
		for (int i=1; i<=N; i++)
		{
			ysaida[i+(k-1)*hops-1] = ysaida[i + (k-1)*hops-1] + Q[i-1][k-1];
		}
	}
	
	//Linear interpolation
	r = ((double)hops-1)/((double)hopa-1);

        for (int n=1; n <= hopa; n++)
        {
            n3 = (((double)n-1)*r+1);
            n1 = floor(n3);
            n2 = ceil(n3);
            yshift[n-1] = ysaida2[n1-1] + ((ysaida2[n2-1]-ysaida2[n1-1]))*(n3 - (double)n1);
		}
		
		fftw_destroy_plan(p);
		fftw_destroy_plan(p2);
	
}
