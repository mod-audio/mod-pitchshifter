#include <fftw3.h>
#include "PitchDetection.h"
#include "Exp.h"
#include "angle.h"
#include <complex>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <armadillo>

using namespace arma;


void FindNote(int N, double *frames, float *frames3, cx_vec *Xa2, cx_vec *Xs2, float *q2, int Qcolumn, fftwf_plan p3, fftwf_plan p4, fftwf_complex *fXa2, fftwf_complex *fXs2, vec *R, vec *NORM, vec *F, vec *AUTO, float fs, int *note, int *oitava)
{

	
	for (int i=1; i<=N; i++)
	{
		frames3[i-1] = frames[i-1];
		frames3[N+i-1] = 0;
	}
	
	fftwf_execute(p3);
	
	for (int i=1; i<=(N + 1); i++)
	{
		Xa2[0](i-1) = cx_float(fXa2[i-1][0], fXa2[i-1][1]);
	}
	
	Xs2[0] = Xa2[0] % conj(Xa2[0]);
	
	for (int i=1; i<=(N + 1); i++)
	{
        fXs2[i-1][0] = real(Xs2[0](i-1));
        fXs2[i-1][1] = imag(Xs2[0](i-1));
	}
	
	fftwf_execute(p4);
	
	for (int i=1; i<=N; i++)
	{
		R[0](i-1) = q2[i-1]/(2*N); 
	}
	
	NORM[0].zeros();
	
	NORM[0](0) = 2*R[0](0);
	
	for (int i=2; i<=N; i++)
	{
		NORM[0](i-1) = NORM[0](i-2) - pow(frames[i-2],2)- pow(frames[N-(i-1)],2);
	}
	
	
	for (int i=1; i<=N; i++)
	{
		F[0](i-1) = 1 -0.05*(i-1)/(N-1);
	}
	
	AUTO[0] = ( 2*F[0] % R[0] )/NORM[0];
	
	int flag = 0;
	
	for (int i=1; (i<=N)&&(flag==0); i++)
	{
		if( AUTO[0](i-1) > 0 )
		{
			AUTO[0](i-1) = 0;
		}
		else
		{
			flag = 1;
		}
	}
	
	uword max_index;
	double fidelity = AUTO[0].max(max_index);
	
	
	if ((fidelity > 0.95) && (fidelity < 1) && ((int)max_index < N-1))
	{
	
		double a = AUTO[0](max_index-1);
		double b = AUTO[0](max_index);
		double c = AUTO[0](max_index+1);
	
		double real_index = max_index + 0.5*(a-c)/(a-2*b+c);
	
		double f = fs/real_index;
		int nota = (int)round( (12/log(2))*log(f/16.35) );
		oitava[0] = floor(nota/12.0);
		note[0] = nota % 12;
	
		//cout << "nota = " << note[0] << " oitava = " << oitava[0] << " fidelity = " << fidelity << "\n";	
		
	}
	
}

void FindStep(int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, int hopa, int Qcolumn, double *s, int *hops)
{
	
		float scale[3][4][3][12];
		
/**********Don't change the pitch on notes out of the scale***************/
	
	//Terça acima

	scale[0][0][0][(0+Tone)%12] = 4;
	scale[0][0][0][(1+Tone)%12] = 0;
	scale[0][0][0][(2+Tone)%12] = 3;
	scale[0][0][0][(3+Tone)%12] = 0;
	scale[0][0][0][(4+Tone)%12] = 3;
	scale[0][0][0][(5+Tone)%12] = 4;
	scale[0][0][0][(6+Tone)%12] = 0;
	scale[0][0][0][(7+Tone)%12] = 4;
	scale[0][0][0][(8+Tone)%12] = 0;
	scale[0][0][0][(9+Tone)%12] = 3;
	scale[0][0][0][(10+Tone)%12] = 0;
	scale[0][0][0][(11+Tone)%12] = 3;
	
	
	scale[0][0][1][(0+Tone)%12] = 3;
	scale[0][0][1][(1+Tone)%12] = 0;
	scale[0][0][1][(2+Tone)%12] = 3;
	scale[0][0][1][(3+Tone)%12] = 4;
	scale[0][0][1][(4+Tone)%12] = 0;
	scale[0][0][1][(5+Tone)%12] = 3;
	scale[0][0][1][(6+Tone)%12] = 0;
	scale[0][0][1][(7+Tone)%12] = 3;
	scale[0][0][1][(8+Tone)%12] = 4;
	scale[0][0][1][(9+Tone)%12] = 0;
	scale[0][0][1][(10+Tone)%12] = 4;
	scale[0][0][1][(11+Tone)%12] = 0;
	
	scale[0][0][2][(0+Tone)%12] = 3;
	scale[0][0][2][(1+Tone)%12] = 0;
	scale[0][0][2][(2+Tone)%12] = 3;
	scale[0][0][2][(3+Tone)%12] = 4;
	scale[0][0][2][(4+Tone)%12] = 0;
	scale[0][0][2][(5+Tone)%12] = 3;
	scale[0][0][2][(6+Tone)%12] = 0;
	scale[0][0][2][(7+Tone)%12] = 4;
	scale[0][0][2][(8+Tone)%12] = 4;
	scale[0][0][2][(9+Tone)%12] = 0;
	scale[0][0][2][(10+Tone)%12] = 0;
	scale[0][0][2][(11+Tone)%12] = 3;
	
	//Sexta acima

	scale[0][1][0][(0+Tone)%12] = 9;
	scale[0][1][0][(1+Tone)%12] = 0;
	scale[0][1][0][(2+Tone)%12] = 9;
	scale[0][1][0][(3+Tone)%12] = 0;
	scale[0][1][0][(4+Tone)%12] = 8;
	scale[0][1][0][(5+Tone)%12] = 9;
	scale[0][1][0][(6+Tone)%12] = 0;
	scale[0][1][0][(7+Tone)%12] = 9;
	scale[0][1][0][(8+Tone)%12] = 0;
	scale[0][1][0][(9+Tone)%12] = 8;
	scale[0][1][0][(10+Tone)%12] = 0;
	scale[0][1][0][(11+Tone)%12] = 8;
	
	
	scale[0][1][1][(0+Tone)%12] = 8;
	scale[0][1][1][(1+Tone)%12] = 0;
	scale[0][1][1][(2+Tone)%12] = 8;
	scale[0][1][1][(3+Tone)%12] = 9;
	scale[0][1][1][(4+Tone)%12] = 0;
	scale[0][1][1][(5+Tone)%12] = 9;
	scale[0][1][1][(6+Tone)%12] = 0;
	scale[0][1][1][(7+Tone)%12] = 8;
	scale[0][1][1][(8+Tone)%12] = 9;
	scale[0][1][1][(9+Tone)%12] = 0;
	scale[0][1][1][(10+Tone)%12] = 9;
	scale[0][1][1][(11+Tone)%12] = 0;
	
	scale[0][1][2][(0+Tone)%12] = 8;
	scale[0][1][2][(1+Tone)%12] = 0;
	scale[0][1][2][(2+Tone)%12] = 9;
	scale[0][1][2][(3+Tone)%12] = 9;
	scale[0][1][2][(4+Tone)%12] = 0;
	scale[0][1][2][(5+Tone)%12] = 9;
	scale[0][1][2][(6+Tone)%12] = 0;
	scale[0][1][2][(7+Tone)%12] = 8;
	scale[0][1][2][(8+Tone)%12] = 9;
	scale[0][1][2][(9+Tone)%12] = 0;
	scale[0][1][2][(10+Tone)%12] = 0;
	scale[0][1][2][(11+Tone)%12] = 8;
	
	//Terça abaixo

	scale[0][2][0][(0+Tone)%12] = -3;
	scale[0][2][0][(1+Tone)%12] = 0;
	scale[0][2][0][(2+Tone)%12] = -3;
	scale[0][2][0][(3+Tone)%12] = 0;
	scale[0][2][0][(4+Tone)%12] = -4;
	scale[0][2][0][(5+Tone)%12] = -3;
	scale[0][2][0][(6+Tone)%12] = 0;
	scale[0][2][0][(7+Tone)%12] = -3;
	scale[0][2][0][(8+Tone)%12] = 0;
	scale[0][2][0][(9+Tone)%12] = -4;
	scale[0][2][0][(10+Tone)%12] = 0;
	scale[0][2][0][(11+Tone)%12] = -4;
	
	
	scale[0][2][1][(0+Tone)%12] = -4;
	scale[0][2][1][(1+Tone)%12] = 0;
	scale[0][2][1][(2+Tone)%12] = -4;
	scale[0][2][1][(3+Tone)%12] = -3;
	scale[0][2][1][(4+Tone)%12] = 0;
	scale[0][2][1][(5+Tone)%12] = -3;
	scale[0][2][1][(6+Tone)%12] = 0;
	scale[0][2][1][(7+Tone)%12] = -4;
	scale[0][2][1][(8+Tone)%12] = -3;
	scale[0][2][1][(9+Tone)%12] = 0;
	scale[0][2][1][(10+Tone)%12] = -3;
	scale[0][2][1][(11+Tone)%12] = 0;
	
	scale[0][2][2][(0+Tone)%12] = -4;
	scale[0][2][2][(1+Tone)%12] = 0;
	scale[0][2][2][(2+Tone)%12] = -3;
	scale[0][2][2][(3+Tone)%12] = -3;
	scale[0][2][2][(4+Tone)%12] = 0;
	scale[0][2][2][(5+Tone)%12] = -3;
	scale[0][2][2][(6+Tone)%12] = 0;
	scale[0][2][2][(7+Tone)%12] = -4;
	scale[0][2][2][(8+Tone)%12] = -3;
	scale[0][2][2][(9+Tone)%12] = 0;
	scale[0][2][2][(10+Tone)%12] = 0;
	scale[0][2][2][(11+Tone)%12] = -4;
	
	//Sexta abaixo

	scale[0][3][0][(0+Tone)%12] = -8;
	scale[0][3][0][(1+Tone)%12] = 0;
	scale[0][3][0][(2+Tone)%12] = -9;
	scale[0][3][0][(3+Tone)%12] = 0;
	scale[0][3][0][(4+Tone)%12] = -9;
	scale[0][3][0][(5+Tone)%12] = -8;
	scale[0][3][0][(6+Tone)%12] = 0;
	scale[0][3][0][(7+Tone)%12] = -8;
	scale[0][3][0][(8+Tone)%12] = 0;
	scale[0][3][0][(9+Tone)%12] = -9;
	scale[0][3][0][(10+Tone)%12] = 0;
	scale[0][3][0][(11+Tone)%12] = -9;	
	
	scale[0][3][1][(0+Tone)%12] = -9;
	scale[0][3][1][(1+Tone)%12] = 0;
	scale[0][3][1][(2+Tone)%12] = -9;
	scale[0][3][1][(3+Tone)%12] = -8;
	scale[0][3][1][(4+Tone)%12] = 0;
	scale[0][3][1][(5+Tone)%12] = -9;
	scale[0][3][1][(6+Tone)%12] = 0;
	scale[0][3][1][(7+Tone)%12] = -9;
	scale[0][3][1][(8+Tone)%12] = -8;
	scale[0][3][1][(9+Tone)%12] = 0;
	scale[0][3][1][(10+Tone)%12] = -8;
	scale[0][3][1][(11+Tone)%12] = 0;

	scale[0][3][2][(0+Tone)%12] = -9;
	scale[0][3][2][(1+Tone)%12] = 0;
	scale[0][3][2][(2+Tone)%12] = -9;
	scale[0][3][2][(3+Tone)%12] = -8;
	scale[0][3][2][(4+Tone)%12] = 0;
	scale[0][3][2][(5+Tone)%12] = -9;
	scale[0][3][2][(6+Tone)%12] = 0;
	scale[0][3][2][(7+Tone)%12] = -9;
	scale[0][3][2][(8+Tone)%12] = -8;
	scale[0][3][2][(9+Tone)%12] = 0;
	scale[0][3][2][(10+Tone)%12] = 0;
	scale[0][3][2][(11+Tone)%12] = -9;
	
	/**********Change the pitch on notes out of the scale to notes on the scale***************/
	
	//Terça acima

	scale[1][0][0][(0+Tone)%12] = 4;
	scale[1][0][0][(1+Tone)%12] = 3;
	scale[1][0][0][(2+Tone)%12] = 3;
	scale[1][0][0][(3+Tone)%12] = 4;
	scale[1][0][0][(4+Tone)%12] = 3;
	scale[1][0][0][(5+Tone)%12] = 4;
	scale[1][0][0][(6+Tone)%12] = 3;
	scale[1][0][0][(7+Tone)%12] = 4;
	scale[1][0][0][(8+Tone)%12] = 3;
	scale[1][0][0][(9+Tone)%12] = 3;
	scale[1][0][0][(10+Tone)%12] = 4;
	scale[1][0][0][(11+Tone)%12] = 3;
	
	
	scale[1][0][1][(0+Tone)%12] = 3;
	scale[1][0][1][(1+Tone)%12] = 4;
	scale[1][0][1][(2+Tone)%12] = 3;
	scale[1][0][1][(3+Tone)%12] = 4;
	scale[1][0][1][(4+Tone)%12] = 3;
	scale[1][0][1][(5+Tone)%12] = 3;
	scale[1][0][1][(6+Tone)%12] = 4;
	scale[1][0][1][(7+Tone)%12] = 3;
	scale[1][0][1][(8+Tone)%12] = 4;
	scale[1][0][1][(9+Tone)%12] = 3;
	scale[1][0][1][(10+Tone)%12] = 4;
	scale[1][0][1][(11+Tone)%12] = 3;
	
	scale[1][0][2][(0+Tone)%12] = 3;
	scale[1][0][2][(1+Tone)%12] = 4;
	scale[1][0][2][(2+Tone)%12] = 3;
	scale[1][0][2][(3+Tone)%12] = 4;
	scale[1][0][2][(4+Tone)%12] = 3;
	scale[1][0][2][(5+Tone)%12] = 3;
	scale[1][0][2][(6+Tone)%12] = 5;
	scale[1][0][2][(7+Tone)%12] = 4;
	scale[1][0][2][(8+Tone)%12] = 4;
	scale[1][0][2][(9+Tone)%12] = 3;
	scale[1][0][2][(10+Tone)%12] = 4;
	scale[1][0][2][(11+Tone)%12] = 3;
	
	//Sexta acima

	scale[1][1][0][(0+Tone)%12] = 9;
	scale[1][1][0][(1+Tone)%12] = 8;
	scale[1][1][0][(2+Tone)%12] = 9;
	scale[1][1][0][(3+Tone)%12] = 8;
	scale[1][1][0][(4+Tone)%12] = 8;
	scale[1][1][0][(5+Tone)%12] = 9;
	scale[1][1][0][(6+Tone)%12] = 8;
	scale[1][1][0][(7+Tone)%12] = 9;
	scale[1][1][0][(8+Tone)%12] = 8;
	scale[1][1][0][(9+Tone)%12] = 8;
	scale[1][1][0][(10+Tone)%12] = 9;
	scale[1][1][0][(11+Tone)%12] = 8;
	
	
	scale[1][1][1][(0+Tone)%12] = 8;
	scale[1][1][1][(1+Tone)%12] = 9;
	scale[1][1][1][(2+Tone)%12] = 8;
	scale[1][1][1][(3+Tone)%12] = 9;
	scale[1][1][1][(4+Tone)%12] = 8;
	scale[1][1][1][(5+Tone)%12] = 9;
	scale[1][1][1][(6+Tone)%12] = 8;
	scale[1][1][1][(7+Tone)%12] = 8;
	scale[1][1][1][(8+Tone)%12] = 9;
	scale[1][1][1][(9+Tone)%12] = 8;
	scale[1][1][1][(10+Tone)%12] = 9;
	scale[1][1][1][(11+Tone)%12] = 8;
	
	scale[1][1][2][(0+Tone)%12] = 8;
	scale[1][1][2][(1+Tone)%12] = 10;
	scale[1][1][2][(2+Tone)%12] = 9;
	scale[1][1][2][(3+Tone)%12] = 9;
	scale[1][1][2][(4+Tone)%12] = 8;
	scale[1][1][2][(5+Tone)%12] = 9;
	scale[1][1][2][(6+Tone)%12] = 8;
	scale[1][1][2][(7+Tone)%12] = 8;
	scale[1][1][2][(8+Tone)%12] = 9;
	scale[1][1][2][(9+Tone)%12] = 8;
	scale[1][1][2][(10+Tone)%12] = 9;
	scale[1][1][2][(11+Tone)%12] = 8;
	
	//Terça abaixo

	scale[1][2][0][(0+Tone)%12] = -3;
	scale[1][2][0][(1+Tone)%12] = -4;
	scale[1][2][0][(2+Tone)%12] = -3;
	scale[1][2][0][(3+Tone)%12] = -4;
	scale[1][2][0][(4+Tone)%12] = -4;
	scale[1][2][0][(5+Tone)%12] = -3;
	scale[1][2][0][(6+Tone)%12] = -4;
	scale[1][2][0][(7+Tone)%12] = -3;
	scale[1][2][0][(8+Tone)%12] = -4;
	scale[1][2][0][(9+Tone)%12] = -4;
	scale[1][2][0][(10+Tone)%12] = -3;
	scale[1][2][0][(11+Tone)%12] = -4;
	
	
	scale[1][2][1][(0+Tone)%12] = -4;
	scale[1][2][1][(1+Tone)%12] = -3;
	scale[1][2][1][(2+Tone)%12] = -4;
	scale[1][2][1][(3+Tone)%12] = -3;
	scale[1][2][1][(4+Tone)%12] = -4;
	scale[1][2][1][(5+Tone)%12] = -3;
	scale[1][2][1][(6+Tone)%12] = -4;
	scale[1][2][1][(7+Tone)%12] = -4;
	scale[1][2][1][(8+Tone)%12] = -3;
	scale[1][2][1][(9+Tone)%12] = -4;
	scale[1][2][1][(10+Tone)%12] = -3;
	scale[1][2][1][(11+Tone)%12] = -4;
	
	scale[1][2][2][(0+Tone)%12] = -4;
	scale[1][2][2][(1+Tone)%12] = -5;
	scale[1][2][2][(2+Tone)%12] = -3;
	scale[1][2][2][(3+Tone)%12] = -3;
	scale[1][2][2][(4+Tone)%12] = -4;
	scale[1][2][2][(5+Tone)%12] = -3;
	scale[1][2][2][(6+Tone)%12] = -4;
	scale[1][2][2][(7+Tone)%12] = -4;
	scale[1][2][2][(8+Tone)%12] = -3;
	scale[1][2][2][(9+Tone)%12] = -4;
	scale[1][2][2][(10+Tone)%12] = -3;
	scale[1][2][2][(11+Tone)%12] = -4;
	
	//Sexta abaixo

	scale[1][3][0][(0+Tone)%12] = -8;
	scale[1][3][0][(1+Tone)%12] = -9;
	scale[1][3][0][(2+Tone)%12] = -9;
	scale[1][3][0][(3+Tone)%12] = -8;
	scale[1][3][0][(4+Tone)%12] = -9;
	scale[1][3][0][(5+Tone)%12] = -8;
	scale[1][3][0][(6+Tone)%12] = -9;
	scale[1][3][0][(7+Tone)%12] = -8;
	scale[1][3][0][(8+Tone)%12] = -9;
	scale[1][3][0][(9+Tone)%12] = -9;
	scale[1][3][0][(10+Tone)%12] = -8;
	scale[1][3][0][(11+Tone)%12] = -9;	
	
	scale[1][3][1][(0+Tone)%12] = -9;
	scale[1][3][1][(1+Tone)%12] = -8;
	scale[1][3][1][(2+Tone)%12] = -9;
	scale[1][3][1][(3+Tone)%12] = -8;
	scale[1][3][1][(4+Tone)%12] = -9;
	scale[1][3][1][(5+Tone)%12] = -9;
	scale[1][3][1][(6+Tone)%12] = -8;
	scale[1][3][1][(7+Tone)%12] = -9;
	scale[1][3][1][(8+Tone)%12] = -8;
	scale[1][3][1][(9+Tone)%12] = -9;
	scale[1][3][1][(10+Tone)%12] = -8;
	scale[1][3][1][(11+Tone)%12] = -9;

	scale[1][3][2][(0+Tone)%12] = -9;
	scale[1][3][2][(1+Tone)%12] = -8;
	scale[1][3][2][(2+Tone)%12] = -9;
	scale[1][3][2][(3+Tone)%12] = -8;
	scale[1][3][2][(4+Tone)%12] = -9;
	scale[1][3][2][(5+Tone)%12] = -9;
	scale[1][3][2][(6+Tone)%12] = -7;
	scale[1][3][2][(7+Tone)%12] = -8;
	scale[1][3][2][(8+Tone)%12] = -8;
	scale[1][3][2][(9+Tone)%12] = -9;
	scale[1][3][2][(10+Tone)%12] = -8;
	scale[1][3][2][(11+Tone)%12] = -9;
	
	/**********Bend Mode***************/
	
	//Terça acima

	scale[2][0][0][(0+Tone)%12] = 4;
	scale[2][0][0][(1+Tone)%12] = 3.5;
	scale[2][0][0][(2+Tone)%12] = 3;
	scale[2][0][0][(3+Tone)%12] = 3;
	scale[2][0][0][(4+Tone)%12] = 3;
	scale[2][0][0][(5+Tone)%12] = 4;
	scale[2][0][0][(6+Tone)%12] = 4;
	scale[2][0][0][(7+Tone)%12] = 4;
	scale[2][0][0][(8+Tone)%12] = 3.5;
	scale[2][0][0][(9+Tone)%12] = 3;
	scale[2][0][0][(10+Tone)%12] = 3;
	scale[2][0][0][(11+Tone)%12] = 3;
	
	
	scale[2][0][1][(0+Tone)%12] = 3;
	scale[2][0][1][(1+Tone)%12] = 3;
	scale[2][0][1][(2+Tone)%12] = 3;
	scale[2][0][1][(3+Tone)%12] = 4;
	scale[2][0][1][(4+Tone)%12] = 3.5;
	scale[2][0][1][(5+Tone)%12] = 3;
	scale[2][0][1][(6+Tone)%12] = 3;
	scale[2][0][1][(7+Tone)%12] = 3;
	scale[2][0][1][(8+Tone)%12] = 4;
	scale[2][0][1][(9+Tone)%12] = 4;
	scale[2][0][1][(10+Tone)%12] = 4;
	scale[2][0][1][(11+Tone)%12] = 3.5;
	
	scale[2][0][2][(0+Tone)%12] = 3;
	scale[2][0][2][(1+Tone)%12] = 3;
	scale[2][0][2][(2+Tone)%12] = 3;
	scale[2][0][2][(3+Tone)%12] = 4;
	scale[2][0][2][(4+Tone)%12] = 3.5;
	scale[2][0][2][(5+Tone)%12] = 3;
	scale[2][0][2][(6+Tone)%12] = 3.5;
	scale[2][0][2][(7+Tone)%12] = 4;
	scale[2][0][2][(8+Tone)%12] = 4;
	scale[2][0][2][(9+Tone)%12] = 3.66;
	scale[2][0][2][(10+Tone)%12] = 3.33;
	scale[2][0][2][(11+Tone)%12] = 3;
	
	//Sexta acima

	scale[2][1][0][(0+Tone)%12] = 9;
	scale[2][1][0][(1+Tone)%12] = 9;
	scale[2][1][0][(2+Tone)%12] = 9;
	scale[2][1][0][(3+Tone)%12] = 8.5;
	scale[2][1][0][(4+Tone)%12] = 8;
	scale[2][1][0][(5+Tone)%12] = 9;
	scale[2][1][0][(6+Tone)%12] = 9;
	scale[2][1][0][(7+Tone)%12] = 9;
	scale[2][1][0][(8+Tone)%12] = 8.5;
	scale[2][1][0][(9+Tone)%12] = 8;
	scale[2][1][0][(10+Tone)%12] = 8;
	scale[2][1][0][(11+Tone)%12] = 8;
	
	
	scale[2][1][1][(0+Tone)%12] = 8;
	scale[2][1][1][(1+Tone)%12] = 8;
	scale[2][1][1][(2+Tone)%12] = 8;
	scale[2][1][1][(3+Tone)%12] = 9;
	scale[2][1][1][(4+Tone)%12] = 9;
	scale[2][1][1][(5+Tone)%12] = 9;
	scale[2][1][1][(6+Tone)%12] = 8.5;
	scale[2][1][1][(7+Tone)%12] = 8;
	scale[2][1][1][(8+Tone)%12] = 9;
	scale[2][1][1][(9+Tone)%12] = 9;
	scale[2][1][1][(10+Tone)%12] = 9;
	scale[2][1][1][(11+Tone)%12] = 8.5;
	
	scale[2][1][2][(0+Tone)%12] = 8;
	scale[2][1][2][(1+Tone)%12] = 8.5;
	scale[2][1][2][(2+Tone)%12] = 9;
	scale[2][1][2][(3+Tone)%12] = 9;
	scale[2][1][2][(4+Tone)%12] = 9;
	scale[2][1][2][(5+Tone)%12] = 9;
	scale[2][1][2][(6+Tone)%12] = 8.5;
	scale[2][1][2][(7+Tone)%12] = 8;
	scale[2][1][2][(8+Tone)%12] = 9;
	scale[2][1][2][(9+Tone)%12] = 8.66;
	scale[2][1][2][(10+Tone)%12] = 8.33;
	scale[2][1][2][(11+Tone)%12] = 8;
	
	//Terça abaixo

	scale[2][2][0][(0+Tone)%12] = -3;
	scale[2][2][0][(1+Tone)%12] = -3;
	scale[2][2][0][(2+Tone)%12] = -3;
	scale[2][2][0][(3+Tone)%12] = -3.5;
	scale[2][2][0][(4+Tone)%12] = -4;
	scale[2][2][0][(5+Tone)%12] = -3;
	scale[2][2][0][(6+Tone)%12] = -3;
	scale[2][2][0][(7+Tone)%12] = -3;
	scale[2][2][0][(8+Tone)%12] = -3.5;
	scale[2][2][0][(9+Tone)%12] = -4;
	scale[2][2][0][(10+Tone)%12] = -4;
	scale[2][2][0][(11+Tone)%12] = -4;
	
	
	scale[2][2][1][(0+Tone)%12] = -4;
	scale[2][2][1][(1+Tone)%12] = -4;
	scale[2][2][1][(2+Tone)%12] = -4;
	scale[2][2][1][(3+Tone)%12] = -3;
	scale[2][2][1][(4+Tone)%12] = -3;
	scale[2][2][1][(5+Tone)%12] = -3;
	scale[2][2][1][(6+Tone)%12] = -3.5;
	scale[2][2][1][(7+Tone)%12] = -4;
	scale[2][2][1][(8+Tone)%12] = -3;
	scale[2][2][1][(9+Tone)%12] = -3;
	scale[2][2][1][(10+Tone)%12] = -3;
	scale[2][2][1][(11+Tone)%12] = -3.5;
	
	scale[2][2][2][(0+Tone)%12] = -4;
	scale[2][2][2][(1+Tone)%12] = -3.5;
	scale[2][2][2][(2+Tone)%12] = -3;
	scale[2][2][2][(3+Tone)%12] = -3;
	scale[2][2][2][(4+Tone)%12] = -3;
	scale[2][2][2][(5+Tone)%12] = -3;
	scale[2][2][2][(6+Tone)%12] = -3.5;
	scale[2][2][2][(7+Tone)%12] = -4;
	scale[2][2][2][(8+Tone)%12] = -3;
	scale[2][2][2][(9+Tone)%12] = -3.33;
	scale[2][2][2][(10+Tone)%12] = -3.66;
	scale[2][2][2][(11+Tone)%12] = -4;
	
	//Sexta abaixo

	scale[2][3][0][(0+Tone)%12] = -8;
	scale[2][3][0][(1+Tone)%12] = -8.5;
	scale[2][3][0][(2+Tone)%12] = -9;
	scale[2][3][0][(3+Tone)%12] = -9;
	scale[2][3][0][(4+Tone)%12] = -9;
	scale[2][3][0][(5+Tone)%12] = -8;
	scale[2][3][0][(6+Tone)%12] = -8;
	scale[2][3][0][(7+Tone)%12] = -8;
	scale[2][3][0][(8+Tone)%12] = -8.5;
	scale[2][3][0][(9+Tone)%12] = -9;
	scale[2][3][0][(10+Tone)%12] = -9;
	scale[2][3][0][(11+Tone)%12] = -9;	
	
	scale[2][3][1][(0+Tone)%12] = -9;
	scale[2][3][1][(1+Tone)%12] = -9;
	scale[2][3][1][(2+Tone)%12] = -9;
	scale[2][3][1][(3+Tone)%12] = -8;
	scale[2][3][1][(4+Tone)%12] = -8.5;
	scale[2][3][1][(5+Tone)%12] = -9;
	scale[2][3][1][(6+Tone)%12] = -9;
	scale[2][3][1][(7+Tone)%12] = -9;
	scale[2][3][1][(8+Tone)%12] = -8;
	scale[2][3][1][(9+Tone)%12] = -8;
	scale[2][3][1][(10+Tone)%12] = -8;
	scale[2][3][1][(11+Tone)%12] = -8.5;

	scale[2][3][2][(0+Tone)%12] = -9;
	scale[2][3][2][(1+Tone)%12] = -9;
	scale[2][3][2][(2+Tone)%12] = -9;
	scale[2][3][2][(3+Tone)%12] = -8;
	scale[2][3][2][(4+Tone)%12] = -8.5;
	scale[2][3][2][(5+Tone)%12] = -9;
	scale[2][3][2][(6+Tone)%12] = -9;
	scale[2][3][2][(7+Tone)%12] = -9;
	scale[2][3][2][(8+Tone)%12] = -8;
	scale[2][3][2][(9+Tone)%12] = -8.33;
	scale[2][3][2][(10+Tone)%12] = -8.66;
	scale[2][3][2][(11+Tone)%12] = -9;
	
	if (Mode < 0) Mode = 0;
	if (Mode > 2) Mode = 2;
	if (Scale < 0) Scale = 0;
	if (Scale > 2) Scale = 2;
	if (Interval < 0) Interval = 0;
	if (Interval > 3) Interval = 3;
	if (LowNote < 0) LowNote = 0;
	if (LowNote > 14) LowNote = 14;
	
	switch (note)
	{
		case 0:
			s[0] = scale[Mode][Interval][Scale][0];
			break;
		case 1:
			s[0] = scale[Mode][Interval][Scale][1];
			break;
		case 2:
			s[0] = scale[Mode][Interval][Scale][2];
			break;
		case 3:
			s[0] = scale[Mode][Interval][Scale][3];
			break;
		case 4:
			s[0] = scale[Mode][Interval][Scale][4];
			break;
		case 5:
			s[0] = scale[Mode][Interval][Scale][5];
			break;
		case 6:
			s[0] = scale[Mode][Interval][Scale][6];
			break;
		case 7:
			s[0] = scale[Mode][Interval][Scale][7];
			break;
		case 8:
			s[0] = scale[Mode][Interval][Scale][8];
			break;
		case 9:
			s[0] = scale[Mode][Interval][Scale][9];
			break;
		case 10:
			s[0] = scale[Mode][Interval][Scale][10];
			break;
		case 11:
			s[0] = scale[Mode][Interval][Scale][11];
			break;
	}
	
	//cout << "LowNote = " << LowNote << " note = " << note << " oitava = " << oitava;
	
	switch (LowNote)
	{
		case 0:
			if ( (oitava == 0) || ( (oitava == 1)&&(note < 10) )  ) s[0] = 0;
			break;
		case 1:
			if ( oitava <=1 ) s[0] = 0;
			break;
		case 2:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 1) )  ) s[0] = 0;
			break;
		case 3:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 2) )  ) s[0] = 0;
			break;
		case 4:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 3) )  ) s[0] = 0;
			break;
		case 5:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 4) )  ) s[0] = 0;
			break;
		case 6:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 5) )  ) s[0] = 0;
			break;
		case 7:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 6) )  ) s[0] = 0;
			break;
		case 8:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 7) )  ) s[0] = 0;
			break;
		case 9:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 8) )  ) s[0] = 0;
			break;
		case 10:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 9) )  ) s[0] = 0;
			break;
		case 11:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 10) )  ) s[0] = 0;
			break;
		case 12:
			if ( (oitava <=1) || ( (oitava == 2)&&(note < 11) )  ) s[0] = 0;
			break;
		case 13:
			if ( (oitava <=2) ) s[0] = 0;
			break;
		case 14:
			if ( (oitava <=2) || ( (oitava == 3)&&(note < 1) )  ) s[0] = 0;
			break;
	}
				
	hops[Qcolumn-1] = round(hopa*(pow(2,(s[0]/12))));
	
	//cout << "nota = " << note << " oitava = " << oitava << " s = " << s[0] << "\n";	
	
}
