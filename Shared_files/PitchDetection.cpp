#include "PitchDetection.h"

PitchDetection::PitchDetection(uint32_t n_samples, int nBuffers) //Constructor
{
	hopa = n_samples;
	N = nBuffers*n_samples;

	frames = fftwf_alloc_real(2*N); memset(frames, 0, 2*N );
	b = new float*[hopa];

	for (int i=0 ; i< nBuffers; i++)
    {
		b[i] = &frames[i*hopa];
	}

	q = fftwf_alloc_real(2*N);	
	fXa = fftwf_alloc_complex(N + 1);
	fXs = fftwf_alloc_complex(N + 1);

	Xa->zeros(N + 1);
	Xs->zeros(N + 1);
	R->zeros(N);
	NORM->zeros(N);
	F->zeros(N);
	AUTO->zeros(N);

	p  = fftwf_plan_dft_r2c_1d(2*N, frames, fXa, FFTW_ESTIMATE);
	p2 = fftwf_plan_dft_c2r_1d(2*N, fXs, q, FFTW_ESTIMATE);
}

PitchDetection::~PitchDetection() //Destructor
{
	delete[] b;

	fftwf_free(frames);
	fftwf_free(q);
	fftwf_free(fXa);
	fftwf_free(fXs);
	
	Xa->clear();
	Xs->clear();
	R->clear();
	NORM->clear();
	F->clear();
	AUTO->clear();
	
	fftwf_destroy_plan(p);
	fftwf_destroy_plan(p2);
}

void PitchDetection::PreProcessing(int nBuffers, float *in)
{
	for (int i=0; i<hopa; i++)
	{
		for (int j=0; j<(nBuffers-1); j++)
		{
			b[j][i] = b[j+1][i];
		}
		b[nBuffers-1][i] = in[i];
	}
}

void PitchDetection::FindNote()
{
	for (int i=0; i<N; i++)
	{
		frames[N+i] = 0;
	}
	
	fftwf_execute(p);
	
	for (int i=0; i<(N + 1); i++)
	{
		Xa[0](i) = cx_float(fXa[i][0], fXa[i][1]);
	}
	
	Xs[0] = Xa[0] % conj(Xa[0]);
	
	for (int i=0; i<(N + 1); i++)
	{
        fXs[i][0] = real(Xs[0](i));
        fXs[i][1] = imag(Xs[0](i));
	}
	
	fftwf_execute(p2);
	
	for (int i=0; i<N; i++)
	{
		R[0](i) = q[i]/(2*N); 
	}
	
	NORM[0].zeros();
	
	NORM[0](0) = 2*R[0](0);
	
	for (int i=1; i<N; i++)
	{
		NORM[0](i) = NORM[0](i-1) - pow(frames[i-1],2)- pow(frames[N-i],2);
	}
	
	
	for (int i=0; i<N; i++)
	{
		F[0](i) = 1 -0.05*i/(N-1);
	}
	
	AUTO[0] = ( 2*F[0] % R[0] )/NORM[0];
	
	int flag = 0;
	
	for (int i=0; (i<N)&&(flag==0); i++)
	{
		if( AUTO[0](i) > 0 )
		{
			AUTO[0](i) = 0;
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
	
		f = fs/real_index;
		int nota = (int)round( (12/log(2))*log(f/16.351597831287414) );
		oitava = floor(nota/12.0);
		note = nota % 12;
	
		//cout << "nota = " << note[0] << " oitava = " << oitava[0] << " fidelity = " << fidelity << "\n";	
		
	}
}



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
		int nota = (int)round( (12/log(2))*log(f/16.351597831287414) );
		oitava[0] = floor(nota/12.0);
		note[0] = nota % 12;
	
		//cout << "nota = " << note[0] << " oitava = " << oitava[0] << " fidelity = " << fidelity << "\n";	
		
	}
	
}

void FindStep2(int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, double *s)
{
	float scale[3][12][3][12];
		
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
	
	//Quarta acima

	scale[0][1][0][(0+Tone)%12] = 5;
	scale[0][1][0][(1+Tone)%12] = 0;
	scale[0][1][0][(2+Tone)%12] = 5;
	scale[0][1][0][(3+Tone)%12] = 0;
	scale[0][1][0][(4+Tone)%12] = 5;
	scale[0][1][0][(5+Tone)%12] = 6;
	scale[0][1][0][(6+Tone)%12] = 0;
	scale[0][1][0][(7+Tone)%12] = 5;
	scale[0][1][0][(8+Tone)%12] = 0;
	scale[0][1][0][(9+Tone)%12] = 5;
	scale[0][1][0][(10+Tone)%12] = 0;
	scale[0][1][0][(11+Tone)%12] = 5;
	scale[0][1][1][(0+Tone)%12] = 5;
	scale[0][1][1][(1+Tone)%12] = 0;
	scale[0][1][1][(2+Tone)%12] = 5;
	scale[0][1][1][(3+Tone)%12] = 5;
	scale[0][1][1][(4+Tone)%12] = 0;
	scale[0][1][1][(5+Tone)%12] = 5;
	scale[0][1][1][(6+Tone)%12] = 0;
	scale[0][1][1][(7+Tone)%12] = 5;
	scale[0][1][1][(8+Tone)%12] = 6;
	scale[0][1][1][(9+Tone)%12] = 0;
	scale[0][1][1][(10+Tone)%12] = 5;
	scale[0][1][1][(11+Tone)%12] = 0;
	scale[0][1][2][(0+Tone)%12] = 5;
	scale[0][1][2][(1+Tone)%12] = 0;
	scale[0][1][2][(2+Tone)%12] = 5;
	scale[0][1][2][(3+Tone)%12] = 5;
	scale[0][1][2][(4+Tone)%12] = 0;
	scale[0][1][2][(5+Tone)%12] = 6;
	scale[0][1][2][(6+Tone)%12] = 0;
	scale[0][1][2][(7+Tone)%12] = 5;
	scale[0][1][2][(8+Tone)%12] = 6;
	scale[0][1][2][(9+Tone)%12] = 0;
	scale[0][1][2][(10+Tone)%12] = 0;
	scale[0][1][2][(11+Tone)%12] = 4;

	//Quinta acima

	scale[0][2][0][(0+Tone)%12] = 7;
	scale[0][2][0][(1+Tone)%12] = 0;
	scale[0][2][0][(2+Tone)%12] = 7;
	scale[0][2][0][(3+Tone)%12] = 0;
	scale[0][2][0][(4+Tone)%12] = 7;
	scale[0][2][0][(5+Tone)%12] = 7;
	scale[0][2][0][(6+Tone)%12] = 0;
	scale[0][2][0][(7+Tone)%12] = 7;
	scale[0][2][0][(8+Tone)%12] = 0;
	scale[0][2][0][(9+Tone)%12] = 7;
	scale[0][2][0][(10+Tone)%12] = 0;
	scale[0][2][0][(11+Tone)%12] = 6;
	scale[0][2][1][(0+Tone)%12] = 7;
	scale[0][2][1][(1+Tone)%12] = 0;
	scale[0][2][1][(2+Tone)%12] = 6;
	scale[0][2][1][(3+Tone)%12] = 7;
	scale[0][2][1][(4+Tone)%12] = 0;
	scale[0][2][1][(5+Tone)%12] = 7;
	scale[0][2][1][(6+Tone)%12] = 0;
	scale[0][2][1][(7+Tone)%12] = 7;
	scale[0][2][1][(8+Tone)%12] = 7;
	scale[0][2][1][(9+Tone)%12] = 0;
	scale[0][2][1][(10+Tone)%12] = 7;
	scale[0][2][1][(11+Tone)%12] = 0;
	scale[0][2][2][(0+Tone)%12] = 7;
	scale[0][2][2][(1+Tone)%12] = 0;
	scale[0][2][2][(2+Tone)%12] = 6;
	scale[0][2][2][(3+Tone)%12] = 8;
	scale[0][2][2][(4+Tone)%12] = 0;
	scale[0][2][2][(5+Tone)%12] = 7;
	scale[0][2][2][(6+Tone)%12] = 0;
	scale[0][2][2][(7+Tone)%12] = 7;
	scale[0][2][2][(8+Tone)%12] = 7;
	scale[0][2][2][(9+Tone)%12] = 0;
	scale[0][2][2][(10+Tone)%12] = 0;
	scale[0][2][2][(11+Tone)%12] = 6;

	//Sexta acima

	scale[0][3][0][(0+Tone)%12] = 9;
	scale[0][3][0][(1+Tone)%12] = 0;
	scale[0][3][0][(2+Tone)%12] = 9;
	scale[0][3][0][(3+Tone)%12] = 0;
	scale[0][3][0][(4+Tone)%12] = 8;
	scale[0][3][0][(5+Tone)%12] = 9;
	scale[0][3][0][(6+Tone)%12] = 0;
	scale[0][3][0][(7+Tone)%12] = 9;
	scale[0][3][0][(8+Tone)%12] = 0;
	scale[0][3][0][(9+Tone)%12] = 8;
	scale[0][3][0][(10+Tone)%12] = 0;
	scale[0][3][0][(11+Tone)%12] = 8;
	scale[0][3][1][(0+Tone)%12] = 8;
	scale[0][3][1][(1+Tone)%12] = 0;
	scale[0][3][1][(2+Tone)%12] = 8;
	scale[0][3][1][(3+Tone)%12] = 9;
	scale[0][3][1][(4+Tone)%12] = 0;
	scale[0][3][1][(5+Tone)%12] = 9;
	scale[0][3][1][(6+Tone)%12] = 0;
	scale[0][3][1][(7+Tone)%12] = 8;
	scale[0][3][1][(8+Tone)%12] = 9;
	scale[0][3][1][(9+Tone)%12] = 0;
	scale[0][3][1][(10+Tone)%12] = 9;
	scale[0][3][1][(11+Tone)%12] = 0;
	scale[0][3][2][(0+Tone)%12] = 8;
	scale[0][3][2][(1+Tone)%12] = 0;
	scale[0][3][2][(2+Tone)%12] = 9;
	scale[0][3][2][(3+Tone)%12] = 9;
	scale[0][3][2][(4+Tone)%12] = 0;
	scale[0][3][2][(5+Tone)%12] = 9;
	scale[0][3][2][(6+Tone)%12] = 0;
	scale[0][3][2][(7+Tone)%12] = 8;
	scale[0][3][2][(8+Tone)%12] = 9;
	scale[0][3][2][(9+Tone)%12] = 0;
	scale[0][3][2][(10+Tone)%12] = 0;
	scale[0][3][2][(11+Tone)%12] = 8;
	
	//Sétima acima

	scale[0][4][0][(0+Tone)%12] = 11;
	scale[0][4][0][(1+Tone)%12] = 0;
	scale[0][4][0][(2+Tone)%12] = 10;
	scale[0][4][0][(3+Tone)%12] = 0;
	scale[0][4][0][(4+Tone)%12] = 10;
	scale[0][4][0][(5+Tone)%12] = 11;
	scale[0][4][0][(6+Tone)%12] = 0;
	scale[0][4][0][(7+Tone)%12] = 10;
	scale[0][4][0][(8+Tone)%12] = 0;
	scale[0][4][0][(9+Tone)%12] = 10;
	scale[0][4][0][(10+Tone)%12] = 0;
	scale[0][4][0][(11+Tone)%12] = 10;
	scale[0][4][1][(0+Tone)%12] = 10;
	scale[0][4][1][(1+Tone)%12] = 0;
	scale[0][4][1][(2+Tone)%12] = 10;
	scale[0][4][1][(3+Tone)%12] = 11;
	scale[0][4][1][(4+Tone)%12] = 0;
	scale[0][4][1][(5+Tone)%12] = 10;
	scale[0][4][1][(6+Tone)%12] = 0;
	scale[0][4][1][(7+Tone)%12] = 10;
	scale[0][4][1][(8+Tone)%12] = 11;
	scale[0][4][1][(9+Tone)%12] = 0;
	scale[0][4][1][(10+Tone)%12] = 10;
	scale[0][4][1][(11+Tone)%12] = 0;
	scale[0][4][2][(0+Tone)%12] = 11;
	scale[0][4][2][(1+Tone)%12] = 0;
	scale[0][4][2][(2+Tone)%12] = 10;
	scale[0][4][2][(3+Tone)%12] = 11;
	scale[0][4][2][(4+Tone)%12] = 0;
	scale[0][4][2][(5+Tone)%12] = 10;
	scale[0][4][2][(6+Tone)%12] = 0;
	scale[0][4][2][(7+Tone)%12] = 10;
	scale[0][4][2][(8+Tone)%12] = 11;
	scale[0][4][2][(9+Tone)%12] = 0;
	scale[0][4][2][(10+Tone)%12] = 0;
	scale[0][4][2][(11+Tone)%12] = 9;

	//Oitava acima

	scale[0][5][0][(0+Tone)%12] = 12;
	scale[0][5][0][(1+Tone)%12] = 0;
	scale[0][5][0][(2+Tone)%12] = 12;
	scale[0][5][0][(3+Tone)%12] = 0;
	scale[0][5][0][(4+Tone)%12] = 12;
	scale[0][5][0][(5+Tone)%12] = 12;
	scale[0][5][0][(6+Tone)%12] = 0;
	scale[0][5][0][(7+Tone)%12] = 12;
	scale[0][5][0][(8+Tone)%12] = 0;
	scale[0][5][0][(9+Tone)%12] = 12;
	scale[0][5][0][(10+Tone)%12] = 0;
	scale[0][5][0][(11+Tone)%12] = 12;
	scale[0][5][1][(0+Tone)%12] = 12;
	scale[0][5][1][(1+Tone)%12] = 0;
	scale[0][5][1][(2+Tone)%12] = 12;
	scale[0][5][1][(3+Tone)%12] = 12;
	scale[0][5][1][(4+Tone)%12] = 0;
	scale[0][5][1][(5+Tone)%12] = 12;
	scale[0][5][1][(6+Tone)%12] = 0;
	scale[0][5][1][(7+Tone)%12] = 12;
	scale[0][5][1][(8+Tone)%12] = 12;
	scale[0][5][1][(9+Tone)%12] = 0;
	scale[0][5][1][(10+Tone)%12] = 12;
	scale[0][5][1][(11+Tone)%12] = 0;
	scale[0][5][2][(0+Tone)%12] = 12;
	scale[0][5][2][(1+Tone)%12] = 0;
	scale[0][5][2][(2+Tone)%12] = 12;
	scale[0][5][2][(3+Tone)%12] = 12;
	scale[0][5][2][(4+Tone)%12] = 0;
	scale[0][5][2][(5+Tone)%12] = 12;
	scale[0][5][2][(6+Tone)%12] = 0;
	scale[0][5][2][(7+Tone)%12] = 12;
	scale[0][5][2][(8+Tone)%12] = 12;
	scale[0][5][2][(9+Tone)%12] = 0;
	scale[0][5][2][(10+Tone)%12] = 0;
	scale[0][5][2][(11+Tone)%12] = 12;

	//Terça abaixo

	scale[0][6][0][(0+Tone)%12] = -3;
	scale[0][6][0][(1+Tone)%12] = 0;
	scale[0][6][0][(2+Tone)%12] = -3;
	scale[0][6][0][(3+Tone)%12] = 0;
	scale[0][6][0][(4+Tone)%12] = -4;
	scale[0][6][0][(5+Tone)%12] = -3;
	scale[0][6][0][(6+Tone)%12] = 0;
	scale[0][6][0][(7+Tone)%12] = -3;
	scale[0][6][0][(8+Tone)%12] = 0;
	scale[0][6][0][(9+Tone)%12] = -4;
	scale[0][6][0][(10+Tone)%12] = 0;
	scale[0][6][0][(11+Tone)%12] = -4;
	scale[0][6][1][(0+Tone)%12] = -4;
	scale[0][6][1][(1+Tone)%12] = 0;
	scale[0][6][1][(2+Tone)%12] = -4;
	scale[0][6][1][(3+Tone)%12] = -3;
	scale[0][6][1][(4+Tone)%12] = 0;
	scale[0][6][1][(5+Tone)%12] = -3;
	scale[0][6][1][(6+Tone)%12] = 0;
	scale[0][6][1][(7+Tone)%12] = -4;
	scale[0][6][1][(8+Tone)%12] = -3;
	scale[0][6][1][(9+Tone)%12] = 0;
	scale[0][6][1][(10+Tone)%12] = -3;
	scale[0][6][1][(11+Tone)%12] = 0;
	scale[0][6][2][(0+Tone)%12] = -4;
	scale[0][6][2][(1+Tone)%12] = 0;
	scale[0][6][2][(2+Tone)%12] = -3;
	scale[0][6][2][(3+Tone)%12] = -3;
	scale[0][6][2][(4+Tone)%12] = 0;
	scale[0][6][2][(5+Tone)%12] = -3;
	scale[0][6][2][(6+Tone)%12] = 0;
	scale[0][6][2][(7+Tone)%12] = -4;
	scale[0][6][2][(8+Tone)%12] = -3;
	scale[0][6][2][(9+Tone)%12] = 0;
	scale[0][6][2][(10+Tone)%12] = 0;
	scale[0][6][2][(11+Tone)%12] = -4;
	
	//Quarta abaixo

	scale[0][7][0][(0+Tone)%12] = -5;
	scale[0][7][0][(1+Tone)%12] = 0;
	scale[0][7][0][(2+Tone)%12] = -5;
	scale[0][7][0][(3+Tone)%12] = 0;
	scale[0][7][0][(4+Tone)%12] = -5;
	scale[0][7][0][(5+Tone)%12] = -5;
	scale[0][7][0][(6+Tone)%12] = 0;
	scale[0][7][0][(7+Tone)%12] = -5;
	scale[0][7][0][(8+Tone)%12] = 0;
	scale[0][7][0][(9+Tone)%12] = -5;
	scale[0][7][0][(10+Tone)%12] = 0;
	scale[0][7][0][(11+Tone)%12] = -6;
	scale[0][7][1][(0+Tone)%12] = -5;
	scale[0][7][1][(1+Tone)%12] = 0;
	scale[0][7][1][(2+Tone)%12] = -6;
	scale[0][7][1][(3+Tone)%12] = -5;
	scale[0][7][1][(4+Tone)%12] = 0;
	scale[0][7][1][(5+Tone)%12] = -5;
	scale[0][7][1][(6+Tone)%12] = 0;
	scale[0][7][1][(7+Tone)%12] = -5;
	scale[0][7][1][(8+Tone)%12] = -5;
	scale[0][7][1][(9+Tone)%12] = 0;
	scale[0][7][1][(10+Tone)%12] = -5;
	scale[0][7][1][(11+Tone)%12] = 0;
	scale[0][7][2][(0+Tone)%12] = -5;
	scale[0][7][2][(1+Tone)%12] = 0;
	scale[0][7][2][(2+Tone)%12] = -6;
	scale[0][7][2][(3+Tone)%12] = -4;
	scale[0][7][2][(4+Tone)%12] = 0;
	scale[0][7][2][(5+Tone)%12] = -5;
	scale[0][7][2][(6+Tone)%12] = 0;
	scale[0][7][2][(7+Tone)%12] = -5;
	scale[0][7][2][(8+Tone)%12] = -5;
	scale[0][7][2][(9+Tone)%12] = 0;
	scale[0][7][2][(10+Tone)%12] = 0;
	scale[0][7][2][(11+Tone)%12] = -6;

	//Quinta abaixo

	scale[0][8][0][(0+Tone)%12] = -7;
	scale[0][8][0][(1+Tone)%12] = 0;
	scale[0][8][0][(2+Tone)%12] = -7;
	scale[0][8][0][(3+Tone)%12] = 0;
	scale[0][8][0][(4+Tone)%12] = -7;
	scale[0][8][0][(5+Tone)%12] = -6;
	scale[0][8][0][(6+Tone)%12] = 0;
	scale[0][8][0][(7+Tone)%12] = -7;
	scale[0][8][0][(8+Tone)%12] = 0;
	scale[0][8][0][(9+Tone)%12] = -7;
	scale[0][8][0][(10+Tone)%12] = 0;
	scale[0][8][0][(11+Tone)%12] = -7;
	scale[0][8][1][(0+Tone)%12] = -7;
	scale[0][8][1][(1+Tone)%12] = 0;
	scale[0][8][1][(2+Tone)%12] = -7;
	scale[0][8][1][(3+Tone)%12] = -7;
	scale[0][8][1][(4+Tone)%12] = 0;
	scale[0][8][1][(5+Tone)%12] = -7;
	scale[0][8][1][(6+Tone)%12] = 0;
	scale[0][8][1][(7+Tone)%12] = -7;
	scale[0][8][1][(8+Tone)%12] = -6;
	scale[0][8][1][(9+Tone)%12] = 0;
	scale[0][8][1][(10+Tone)%12] = -7;
	scale[0][8][1][(11+Tone)%12] = 0;
	scale[0][8][2][(0+Tone)%12] = -7;
	scale[0][8][2][(1+Tone)%12] = 0;
	scale[0][8][2][(2+Tone)%12] = -7;
	scale[0][8][2][(3+Tone)%12] = -7;
	scale[0][8][2][(4+Tone)%12] = 0;
	scale[0][8][2][(5+Tone)%12] = -6;
	scale[0][8][2][(6+Tone)%12] = 0;
	scale[0][8][2][(7+Tone)%12] = -7;
	scale[0][8][2][(8+Tone)%12] = -6;
	scale[0][8][2][(9+Tone)%12] = 0;
	scale[0][8][2][(10+Tone)%12] = 0;
	scale[0][8][2][(11+Tone)%12] = -8;

	//Sexta abaixo

	scale[0][9][0][(0+Tone)%12] = -8;
	scale[0][9][0][(1+Tone)%12] = 0;
	scale[0][9][0][(2+Tone)%12] = -9;
	scale[0][9][0][(3+Tone)%12] = 0;
	scale[0][9][0][(4+Tone)%12] = -9;
	scale[0][9][0][(5+Tone)%12] = -8;
	scale[0][9][0][(6+Tone)%12] = 0;
	scale[0][9][0][(7+Tone)%12] = -8;
	scale[0][9][0][(8+Tone)%12] = 0;
	scale[0][9][0][(9+Tone)%12] = -9;
	scale[0][9][0][(10+Tone)%12] = 0;
	scale[0][9][0][(11+Tone)%12] = -9;	
	scale[0][9][1][(0+Tone)%12] = -9;
	scale[0][9][1][(1+Tone)%12] = 0;
	scale[0][9][1][(2+Tone)%12] = -9;
	scale[0][9][1][(3+Tone)%12] = -8;
	scale[0][9][1][(4+Tone)%12] = 0;
	scale[0][9][1][(5+Tone)%12] = -9;
	scale[0][9][1][(6+Tone)%12] = 0;
	scale[0][9][1][(7+Tone)%12] = -9;
	scale[0][9][1][(8+Tone)%12] = -8;
	scale[0][9][1][(9+Tone)%12] = 0;
	scale[0][9][1][(10+Tone)%12] = -8;
	scale[0][9][1][(11+Tone)%12] = 0;
	scale[0][9][2][(0+Tone)%12] = -9;
	scale[0][9][2][(1+Tone)%12] = 0;
	scale[0][9][2][(2+Tone)%12] = -9;
	scale[0][9][2][(3+Tone)%12] = -8;
	scale[0][9][2][(4+Tone)%12] = 0;
	scale[0][9][2][(5+Tone)%12] = -9;
	scale[0][9][2][(6+Tone)%12] = 0;
	scale[0][9][2][(7+Tone)%12] = -9;
	scale[0][9][2][(8+Tone)%12] = -8;
	scale[0][9][2][(9+Tone)%12] = 0;
	scale[0][9][2][(10+Tone)%12] = 0;
	scale[0][9][2][(11+Tone)%12] = -9;

	//Sétima abaixo

	scale[0][10][0][(0+Tone)%12] = -10;
	scale[0][10][0][(1+Tone)%12] = 0;
	scale[0][10][0][(2+Tone)%12] = -10;
	scale[0][10][0][(3+Tone)%12] = 0;
	scale[0][10][0][(4+Tone)%12] = -11;
	scale[0][10][0][(5+Tone)%12] = -10;
	scale[0][10][0][(6+Tone)%12] = 0;
	scale[0][10][0][(7+Tone)%12] = -10;
	scale[0][10][0][(8+Tone)%12] = 0;
	scale[0][10][0][(9+Tone)%12] = -10;
	scale[0][10][0][(10+Tone)%12] = 0;
	scale[0][10][0][(11+Tone)%12] = -11;
	scale[0][10][1][(0+Tone)%12] = -10;
	scale[0][10][1][(1+Tone)%12] = 0;
	scale[0][10][1][(2+Tone)%12] = -11;
	scale[0][10][1][(3+Tone)%12] = -10;
	scale[0][10][1][(4+Tone)%12] = 0;
	scale[0][10][1][(5+Tone)%12] = -10;
	scale[0][10][1][(6+Tone)%12] = 0;
	scale[0][10][1][(7+Tone)%12] = -11;
	scale[0][10][1][(8+Tone)%12] = -10;
	scale[0][10][1][(9+Tone)%12] = 0;
	scale[0][10][1][(10+Tone)%12] = -10;
	scale[0][10][1][(11+Tone)%12] = 0;
	scale[0][10][2][(0+Tone)%12] = -10;
	scale[0][10][2][(1+Tone)%12] = 0;
	scale[0][10][2][(2+Tone)%12] = -11;
	scale[0][10][2][(3+Tone)%12] = -10;
	scale[0][10][2][(4+Tone)%12] = 0;
	scale[0][10][2][(5+Tone)%12] = -10;
	scale[0][10][2][(6+Tone)%12] = 0;
	scale[0][10][2][(7+Tone)%12] = -11;
	scale[0][10][2][(8+Tone)%12] = -9;
	scale[0][10][2][(9+Tone)%12] = 0;
	scale[0][10][2][(10+Tone)%12] = 0;
	scale[0][10][2][(11+Tone)%12] = -11;

	//Oitava abaixo

	scale[0][11][0][(0+Tone)%12] = -12;
	scale[0][11][0][(1+Tone)%12] = 0;
	scale[0][11][0][(2+Tone)%12] = -12;
	scale[0][11][0][(3+Tone)%12] = 0;
	scale[0][11][0][(4+Tone)%12] = -12;
	scale[0][11][0][(5+Tone)%12] = -12;
	scale[0][11][0][(6+Tone)%12] = 0;
	scale[0][11][0][(7+Tone)%12] = -12;
	scale[0][11][0][(8+Tone)%12] = 0;
	scale[0][11][0][(9+Tone)%12] = -12;
	scale[0][11][0][(10+Tone)%12] = 0;
	scale[0][11][0][(11+Tone)%12] = -12;
	scale[0][11][1][(0+Tone)%12] = -12;
	scale[0][11][1][(1+Tone)%12] = 0;
	scale[0][11][1][(2+Tone)%12] = -12;
	scale[0][11][1][(3+Tone)%12] = -12;
	scale[0][11][1][(4+Tone)%12] = 0;
	scale[0][11][1][(5+Tone)%12] = -12;
	scale[0][11][1][(6+Tone)%12] = 0;
	scale[0][11][1][(7+Tone)%12] = -12;
	scale[0][11][1][(8+Tone)%12] = -12;
	scale[0][11][1][(9+Tone)%12] = 0;
	scale[0][11][1][(10+Tone)%12] = -12;
	scale[0][11][1][(11+Tone)%12] = 0;
	scale[0][11][2][(0+Tone)%12] = -12;
	scale[0][11][2][(1+Tone)%12] = 0;
	scale[0][11][2][(2+Tone)%12] = -12;
	scale[0][11][2][(3+Tone)%12] = -12;
	scale[0][11][2][(4+Tone)%12] = 0;
	scale[0][11][2][(5+Tone)%12] = -12;
	scale[0][11][2][(6+Tone)%12] = 0;
	scale[0][11][2][(7+Tone)%12] = -12;
	scale[0][11][2][(8+Tone)%12] = -12;
	scale[0][11][2][(9+Tone)%12] = 0;
	scale[0][11][2][(10+Tone)%12] = 0;
	scale[0][11][2][(11+Tone)%12] = -12;

	
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
	
	//Quarta acima

	scale[1][1][0][(0+Tone)%12] = 5;
	scale[1][1][0][(1+Tone)%12] = 4;
	scale[1][1][0][(2+Tone)%12] = 5;
	scale[1][1][0][(3+Tone)%12] = 4;
	scale[1][1][0][(4+Tone)%12] = 5;
	scale[1][1][0][(5+Tone)%12] = 6;
	scale[1][1][0][(6+Tone)%12] = 5;
	scale[1][1][0][(7+Tone)%12] = 5;
	scale[1][1][0][(8+Tone)%12] = 4;
	scale[1][1][0][(9+Tone)%12] = 5;
	scale[1][1][0][(10+Tone)%12] = 4;
	scale[1][1][0][(11+Tone)%12] = 5;
	scale[1][1][1][(0+Tone)%12] = 5;
	scale[1][1][1][(1+Tone)%12] = 4;
	scale[1][1][1][(2+Tone)%12] = 5;
	scale[1][1][1][(3+Tone)%12] = 5;
	scale[1][1][1][(4+Tone)%12] = 4;
	scale[1][1][1][(5+Tone)%12] = 5;
	scale[1][1][1][(6+Tone)%12] = 4;
	scale[1][1][1][(7+Tone)%12] = 5;
	scale[1][1][1][(8+Tone)%12] = 6;
	scale[1][1][1][(9+Tone)%12] = 5;
	scale[1][1][1][(10+Tone)%12] = 5;
	scale[1][1][1][(11+Tone)%12] = 4;
	scale[1][1][2][(0+Tone)%12] = 5;
	scale[1][1][2][(1+Tone)%12] = 4;
	scale[1][1][2][(2+Tone)%12] = 5;
	scale[1][1][2][(3+Tone)%12] = 5;
	scale[1][1][2][(4+Tone)%12] = 4;
	scale[1][1][2][(5+Tone)%12] = 6;
	scale[1][1][2][(6+Tone)%12] = 5;
	scale[1][1][2][(7+Tone)%12] = 5;
	scale[1][1][2][(8+Tone)%12] = 6;
	scale[1][1][2][(9+Tone)%12] = 5;
	scale[1][1][2][(11+Tone)%12] = 4;
	scale[1][1][2][(10+Tone)%12] = 4;

	//Quinta acima

	scale[1][2][0][(0+Tone)%12] = 7;
	scale[1][2][0][(1+Tone)%12] = 6;
	scale[1][2][0][(2+Tone)%12] = 7;
	scale[1][2][0][(3+Tone)%12] = 6;
	scale[1][2][0][(4+Tone)%12] = 7;
	scale[1][2][0][(5+Tone)%12] = 7;
	scale[1][2][0][(6+Tone)%12] = 6;
	scale[1][2][0][(7+Tone)%12] = 7;
	scale[1][2][0][(8+Tone)%12] = 6;
	scale[1][2][0][(9+Tone)%12] = 7;
	scale[1][2][0][(10+Tone)%12] = 6;
	scale[1][2][0][(11+Tone)%12] = 6;
	scale[1][2][1][(0+Tone)%12] = 7;
	scale[1][2][1][(1+Tone)%12] = 6;
	scale[1][2][1][(2+Tone)%12] = 6;
	scale[1][2][1][(3+Tone)%12] = 7;
	scale[1][2][1][(4+Tone)%12] = 6;
	scale[1][2][1][(5+Tone)%12] = 7;
	scale[1][2][0][(0+Tone)%12] = 7;
	scale[1][2][1][(6+Tone)%12] = 6;
	scale[1][2][1][(7+Tone)%12] = 7;
	scale[1][2][1][(8+Tone)%12] = 7;
	scale[1][2][1][(9+Tone)%12] = 6;
	scale[1][2][1][(10+Tone)%12] = 7;
	scale[1][2][1][(11+Tone)%12] = 6;
	scale[1][2][2][(0+Tone)%12] = 7;
	scale[1][2][2][(1+Tone)%12] = 6;
	scale[1][2][2][(2+Tone)%12] = 6;
	scale[1][2][2][(3+Tone)%12] = 8;
	scale[1][2][2][(4+Tone)%12] = 7;
	scale[1][2][2][(5+Tone)%12] = 7;
	scale[1][2][2][(6+Tone)%12] = 6;
	scale[1][2][2][(7+Tone)%12] = 7;
	scale[1][2][2][(8+Tone)%12] = 7;
	scale[1][2][2][(9+Tone)%12] = 6;
	scale[1][2][2][(10+Tone)%12] = 5;
	scale[1][2][2][(11+Tone)%12] = 6;

	//Sexta acima

	scale[1][3][0][(0+Tone)%12] = 9;
	scale[1][3][0][(1+Tone)%12] = 8;
	scale[1][3][0][(2+Tone)%12] = 9;
	scale[1][3][0][(3+Tone)%12] = 8;
	scale[1][3][0][(4+Tone)%12] = 8;
	scale[1][3][0][(5+Tone)%12] = 9;
	scale[1][3][0][(6+Tone)%12] = 8;
	scale[1][3][0][(7+Tone)%12] = 9;
	scale[1][3][0][(8+Tone)%12] = 8;
	scale[1][3][0][(9+Tone)%12] = 8;
	scale[1][3][0][(10+Tone)%12] = 9;
	scale[1][3][0][(11+Tone)%12] = 8;
	scale[1][3][1][(0+Tone)%12] = 8;
	scale[1][3][1][(1+Tone)%12] = 9;
	scale[1][3][1][(2+Tone)%12] = 8;
	scale[1][3][1][(3+Tone)%12] = 9;
	scale[1][3][1][(4+Tone)%12] = 8;
	scale[1][3][1][(5+Tone)%12] = 9;
	scale[1][3][1][(6+Tone)%12] = 8;
	scale[1][3][1][(7+Tone)%12] = 8;
	scale[1][3][1][(8+Tone)%12] = 9;
	scale[1][3][1][(9+Tone)%12] = 8;
	scale[1][3][1][(10+Tone)%12] = 9;
	scale[1][3][1][(11+Tone)%12] = 8;
	scale[1][3][2][(0+Tone)%12] = 8;
	scale[1][3][2][(1+Tone)%12] = 10;
	scale[1][3][2][(2+Tone)%12] = 9;
	scale[1][3][2][(3+Tone)%12] = 9;
	scale[1][3][2][(4+Tone)%12] = 8;
	scale[1][3][2][(5+Tone)%12] = 9;
	scale[1][3][2][(6+Tone)%12] = 8;
	scale[1][3][2][(7+Tone)%12] = 8;
	scale[1][3][2][(8+Tone)%12] = 9;
	scale[1][3][2][(9+Tone)%12] = 8;
	scale[1][3][2][(10+Tone)%12] = 9;
	scale[1][3][2][(11+Tone)%12] = 8;
	
	//Sétima acima

	scale[1][4][0][(0+Tone)%12] = 11;
	scale[1][4][0][(1+Tone)%12] = 11;
	scale[1][4][0][(2+Tone)%12] = 10;
	scale[1][4][0][(3+Tone)%12] = 11;
	scale[1][4][0][(4+Tone)%12] = 10;
	scale[1][4][0][(5+Tone)%12] = 11;
	scale[1][4][0][(6+Tone)%12] = 11;
	scale[1][4][0][(7+Tone)%12] = 10;
	scale[1][4][0][(8+Tone)%12] = 11;
	scale[1][4][0][(9+Tone)%12] = 10;
	scale[1][4][0][(10+Tone)%12] = 11;
	scale[1][4][0][(11+Tone)%12] = 10;
	scale[1][4][1][(0+Tone)%12] = 10;
	scale[1][4][1][(1+Tone)%12] = 11;
	scale[1][4][1][(2+Tone)%12] = 10;
	scale[1][4][1][(3+Tone)%12] = 11;
	scale[1][4][1][(4+Tone)%12] = 11;
	scale[1][4][1][(5+Tone)%12] = 10;
	scale[1][4][1][(6+Tone)%12] = 11;
	scale[1][4][1][(7+Tone)%12] = 10;
	scale[1][4][1][(8+Tone)%12] = 11;
	scale[1][4][1][(9+Tone)%12] = 11;
	scale[1][4][1][(10+Tone)%12] = 10;
	scale[1][4][1][(11+Tone)%12] = 11;
	scale[1][4][2][(0+Tone)%12] = 11;
	scale[1][4][2][(1+Tone)%12] = 11;
	scale[1][4][2][(2+Tone)%12] = 10;
	scale[1][4][2][(3+Tone)%12] = 11;
	scale[1][4][2][(4+Tone)%12] = 11;
	scale[1][4][2][(5+Tone)%12] = 10;
	scale[1][4][2][(6+Tone)%12] = 11;
	scale[1][4][2][(7+Tone)%12] = 10;
	scale[1][4][2][(8+Tone)%12] = 11;
	scale[1][4][2][(9+Tone)%12] = 11;
	scale[1][4][2][(10+Tone)%12] = 10;
	scale[1][4][2][(11+Tone)%12] = 9;

	//Oitava acima

	scale[1][5][0][(0+Tone)%12] = 12;
	scale[1][5][0][(1+Tone)%12] = 12;
	scale[1][5][0][(2+Tone)%12] = 12;
	scale[1][5][0][(3+Tone)%12] = 12;
	scale[1][5][0][(4+Tone)%12] = 12;
	scale[1][5][0][(5+Tone)%12] = 12;
	scale[1][5][0][(6+Tone)%12] = 12;
	scale[1][5][0][(7+Tone)%12] = 12;
	scale[1][5][0][(8+Tone)%12] = 12;
	scale[1][5][0][(9+Tone)%12] = 12;
	scale[1][5][0][(10+Tone)%12] = 12;
	scale[1][5][0][(11+Tone)%12] = 12;
	scale[1][5][1][(0+Tone)%12] = 12;
	scale[1][5][1][(1+Tone)%12] = 12;
	scale[1][5][1][(2+Tone)%12] = 12;
	scale[1][5][1][(3+Tone)%12] = 12;
	scale[1][5][1][(4+Tone)%12] = 12;
	scale[1][5][1][(5+Tone)%12] = 12;
	scale[1][5][1][(6+Tone)%12] = 12;
	scale[1][5][1][(7+Tone)%12] = 12;
	scale[1][5][1][(8+Tone)%12] = 12;
	scale[1][5][1][(9+Tone)%12] = 12;
	scale[1][5][1][(10+Tone)%12] = 12;
	scale[1][5][1][(11+Tone)%12] = 12;
	scale[1][5][2][(0+Tone)%12] = 12;
	scale[1][5][2][(1+Tone)%12] = 12;
	scale[1][5][2][(2+Tone)%12] = 12;
	scale[1][5][2][(3+Tone)%12] = 12;
	scale[1][5][2][(4+Tone)%12] = 12;
	scale[1][5][2][(5+Tone)%12] = 12;
	scale[1][5][2][(6+Tone)%12] = 12;
	scale[1][5][2][(7+Tone)%12] = 12;
	scale[1][5][2][(8+Tone)%12] = 12;
	scale[1][5][2][(9+Tone)%12] = 12;
	scale[1][5][2][(10+Tone)%12] = 12;
	scale[1][5][2][(11+Tone)%12] = 12;

	//Terça abaixo

	scale[1][6][0][(0+Tone)%12] = -3;
	scale[1][6][0][(1+Tone)%12] = -4;
	scale[1][6][0][(2+Tone)%12] = -3;
	scale[1][6][0][(3+Tone)%12] = -4;
	scale[1][6][0][(4+Tone)%12] = -4;
	scale[1][6][0][(5+Tone)%12] = -3;
	scale[1][6][0][(6+Tone)%12] = -4;
	scale[1][6][0][(7+Tone)%12] = -3;
	scale[1][6][0][(8+Tone)%12] = -4;
	scale[1][6][0][(9+Tone)%12] = -4;
	scale[1][6][0][(10+Tone)%12] = -3;
	scale[1][6][0][(11+Tone)%12] = -4;
	scale[1][6][1][(0+Tone)%12] = -4;
	scale[1][6][1][(1+Tone)%12] = -3;
	scale[1][6][1][(2+Tone)%12] = -4;
	scale[1][6][1][(3+Tone)%12] = -3;
	scale[1][6][1][(4+Tone)%12] = -4;
	scale[1][6][1][(5+Tone)%12] = -3;
	scale[1][6][1][(6+Tone)%12] = -4;
	scale[1][6][1][(7+Tone)%12] = -4;
	scale[1][6][1][(8+Tone)%12] = -3;
	scale[1][6][1][(9+Tone)%12] = -4;
	scale[1][6][1][(10+Tone)%12] = -3;
	scale[1][6][1][(11+Tone)%12] = -4;
	scale[1][6][2][(0+Tone)%12] = -4;
	scale[1][6][2][(1+Tone)%12] = -5;
	scale[1][6][2][(2+Tone)%12] = -3;
	scale[1][6][2][(3+Tone)%12] = -3;
	scale[1][6][2][(4+Tone)%12] = -4;
	scale[1][6][2][(5+Tone)%12] = -3;
	scale[1][6][2][(6+Tone)%12] = -4;
	scale[1][6][2][(7+Tone)%12] = -4;
	scale[1][6][2][(8+Tone)%12] = -3;
	scale[1][6][2][(9+Tone)%12] = -4;
	scale[1][6][2][(10+Tone)%12] = -3;
	scale[1][6][2][(11+Tone)%12] = -4;
	
	//Quarta abaixo

	scale[1][7][0][(0+Tone)%12] = -5;
	scale[1][7][0][(1+Tone)%12] = -6;
	scale[1][7][0][(2+Tone)%12] = -5;
	scale[1][7][0][(3+Tone)%12] = -6;
	scale[1][7][0][(4+Tone)%12] = -5;
	scale[1][7][0][(5+Tone)%12] = -5;
	scale[1][7][0][(6+Tone)%12] = -6;
	scale[1][7][0][(7+Tone)%12] = -5;
	scale[1][7][0][(8+Tone)%12] = -6;
	scale[1][7][0][(9+Tone)%12] = -5;
	scale[1][7][0][(10+Tone)%12] = -6;
	scale[1][7][0][(11+Tone)%12] = -6;
	scale[1][7][1][(0+Tone)%12] = -5;
	scale[1][7][1][(1+Tone)%12] = -6;
	scale[1][7][1][(2+Tone)%12] = -6;
	scale[1][7][1][(3+Tone)%12] = -5;
	scale[1][7][1][(4+Tone)%12] = -6;
	scale[1][7][1][(5+Tone)%12] = -5;
	scale[1][7][1][(6+Tone)%12] = -6;
	scale[1][7][1][(7+Tone)%12] = -5;
	scale[1][7][1][(8+Tone)%12] = -5;
	scale[1][7][1][(9+Tone)%12] = -6;
	scale[1][7][1][(10+Tone)%12] = -5;
	scale[1][7][1][(11+Tone)%12] = -6;
	scale[1][7][2][(0+Tone)%12] = -5;
	scale[1][7][2][(1+Tone)%12] = -6;
	scale[1][7][2][(2+Tone)%12] = -6;
	scale[1][7][2][(3+Tone)%12] = -4;
	scale[1][7][2][(4+Tone)%12] = -5;
	scale[1][7][2][(5+Tone)%12] = -5;
	scale[1][7][2][(6+Tone)%12] = -6;
	scale[1][7][2][(7+Tone)%12] = -5;
	scale[1][7][2][(8+Tone)%12] = -5;
	scale[1][7][2][(9+Tone)%12] = -6;
	scale[1][7][2][(10+Tone)%12] = -7;
	scale[1][7][2][(11+Tone)%12] = -6;

	//Quinta abaixo

	scale[1][8][0][(0+Tone)%12] = -7;
	scale[1][8][0][(1+Tone)%12] = -8;
	scale[1][8][0][(2+Tone)%12] = -7;
	scale[1][8][0][(3+Tone)%12] = -8;
	scale[1][8][0][(4+Tone)%12] = -7;
	scale[1][8][0][(5+Tone)%12] = -6;
	scale[1][8][0][(6+Tone)%12] = -7;
	scale[1][8][0][(7+Tone)%12] = -7;
	scale[1][8][0][(8+Tone)%12] = -8;
	scale[1][8][0][(9+Tone)%12] = -7;
	scale[1][8][0][(10+Tone)%12] = -8;
	scale[1][8][0][(11+Tone)%12] = -7;
	scale[1][8][1][(0+Tone)%12] = -7;
	scale[1][8][1][(1+Tone)%12] = -8;
	scale[1][8][1][(2+Tone)%12] = -7;
	scale[1][8][1][(3+Tone)%12] = -7;
	scale[1][8][1][(4+Tone)%12] = -8;
	scale[1][8][1][(5+Tone)%12] = -7;
	scale[1][8][1][(6+Tone)%12] = -8;
	scale[1][8][1][(7+Tone)%12] = -7;
	scale[1][8][1][(8+Tone)%12] = -6;
	scale[1][8][1][(9+Tone)%12] = -7;
	scale[1][8][1][(10+Tone)%12] = -7;
	scale[1][8][1][(11+Tone)%12] = -8;
	scale[1][8][2][(0+Tone)%12] = -7;
	scale[1][8][2][(1+Tone)%12] = -8;
	scale[1][8][2][(2+Tone)%12] = -7;
	scale[1][8][2][(3+Tone)%12] = -7;
	scale[1][8][2][(4+Tone)%12] = -8;
	scale[1][8][2][(5+Tone)%12] = -6;
	scale[1][8][2][(6+Tone)%12] = -7;
	scale[1][8][2][(7+Tone)%12] = -7;
	scale[1][8][2][(8+Tone)%12] = -6;
	scale[1][8][2][(9+Tone)%12] = -7;
	scale[1][8][2][(11+Tone)%12] = -8;
	scale[1][8][2][(10+Tone)%12] = -8;

	//Sexta abaixo

	scale[1][9][0][(0+Tone)%12] = -8;
	scale[1][9][0][(1+Tone)%12] = -9;
	scale[1][9][0][(2+Tone)%12] = -9;
	scale[1][9][0][(3+Tone)%12] = -8;
	scale[1][9][0][(4+Tone)%12] = -9;
	scale[1][9][0][(5+Tone)%12] = -8;
	scale[1][9][0][(6+Tone)%12] = -9;
	scale[1][9][0][(7+Tone)%12] = -8;
	scale[1][9][0][(8+Tone)%12] = -9;
	scale[1][9][0][(9+Tone)%12] = -9;
	scale[1][9][0][(10+Tone)%12] = -8;
	scale[1][9][0][(11+Tone)%12] = -9;	
	scale[1][9][1][(0+Tone)%12] = -9;
	scale[1][9][1][(1+Tone)%12] = -8;
	scale[1][9][1][(2+Tone)%12] = -9;
	scale[1][9][1][(3+Tone)%12] = -8;
	scale[1][9][1][(4+Tone)%12] = -9;
	scale[1][9][1][(5+Tone)%12] = -9;
	scale[1][9][1][(6+Tone)%12] = -8;
	scale[1][9][1][(7+Tone)%12] = -9;
	scale[1][9][1][(8+Tone)%12] = -8;
	scale[1][9][1][(9+Tone)%12] = -9;
	scale[1][9][1][(10+Tone)%12] = -8;
	scale[1][9][1][(11+Tone)%12] = -9;
	scale[1][9][2][(0+Tone)%12] = -9;
	scale[1][9][2][(1+Tone)%12] = -8;
	scale[1][9][2][(2+Tone)%12] = -9;
	scale[1][9][2][(3+Tone)%12] = -8;
	scale[1][9][2][(4+Tone)%12] = -9;
	scale[1][9][2][(5+Tone)%12] = -9;
	scale[1][9][2][(6+Tone)%12] = -7;
	scale[1][9][2][(7+Tone)%12] = -8;
	scale[1][9][2][(8+Tone)%12] = -8;
	scale[1][9][2][(9+Tone)%12] = -9;
	scale[1][9][2][(10+Tone)%12] = -8;
	scale[1][9][2][(11+Tone)%12] = -9;

	//Sétima abaixo

	scale[1][10][0][(0+Tone)%12] = -10;
	scale[1][10][0][(1+Tone)%12] = -11;
	scale[1][10][0][(2+Tone)%12] = -10;
	scale[1][10][0][(3+Tone)%12] = -11;
	scale[1][10][0][(4+Tone)%12] = -11;
	scale[1][10][0][(5+Tone)%12] = -10;
	scale[1][10][0][(6+Tone)%12] = -11;
	scale[1][10][0][(7+Tone)%12] = -10;
	scale[1][10][0][(8+Tone)%12] = -11;
	scale[1][10][0][(9+Tone)%12] = -10;
	scale[1][10][0][(10+Tone)%12] = -11;
	scale[1][10][0][(11+Tone)%12] = -11;
	scale[1][10][1][(0+Tone)%12] = -10;
	scale[1][10][1][(1+Tone)%12] = -11;
	scale[1][10][1][(2+Tone)%12] = -11;
	scale[1][10][1][(3+Tone)%12] = -10;
	scale[1][10][1][(4+Tone)%12] = -11;
	scale[1][10][1][(5+Tone)%12] = -10;
	scale[1][10][1][(6+Tone)%12] = -11;
	scale[1][10][1][(7+Tone)%12] = -11;
	scale[1][10][1][(8+Tone)%12] = -10;
	scale[1][10][1][(9+Tone)%12] = -11;
	scale[1][10][1][(10+Tone)%12] = -10;
	scale[1][10][1][(11+Tone)%12] = -11;
	scale[1][10][2][(0+Tone)%12] = -10;
	scale[1][10][2][(1+Tone)%12] = -11;
	scale[1][10][2][(2+Tone)%12] = -11;
	scale[1][10][2][(3+Tone)%12] = -10;
	scale[1][10][2][(4+Tone)%12] = -11;
	scale[1][10][2][(5+Tone)%12] = -10;
	scale[1][10][2][(6+Tone)%12] = -11;
	scale[1][10][2][(7+Tone)%12] = -11;
	scale[1][10][2][(8+Tone)%12] = -9;
	scale[1][10][2][(9+Tone)%12] = -10;
	scale[1][10][2][(10+Tone)%12] = -11;
	scale[1][10][2][(11+Tone)%12] = -11;
	
	//Oitava abaixo

	scale[1][11][0][(0+Tone)%12] = -12;
	scale[1][11][0][(1+Tone)%12] = -12;
	scale[1][11][0][(2+Tone)%12] = -12;
	scale[1][11][0][(3+Tone)%12] = -12;
	scale[1][11][0][(4+Tone)%12] = -12;
	scale[1][11][0][(5+Tone)%12] = -12;
	scale[1][11][0][(6+Tone)%12] = -12;
	scale[1][11][0][(7+Tone)%12] = -12;
	scale[1][11][0][(8+Tone)%12] = -12;
	scale[1][11][0][(9+Tone)%12] = -12;
	scale[1][11][0][(10+Tone)%12] = -12;
	scale[1][11][0][(11+Tone)%12] = -12;
	scale[1][11][1][(0+Tone)%12] = -12;
	scale[1][11][1][(1+Tone)%12] = -12;
	scale[1][11][1][(2+Tone)%12] = -12;
	scale[1][11][1][(3+Tone)%12] = -12;
	scale[1][11][1][(4+Tone)%12] = -12;
	scale[1][11][1][(5+Tone)%12] = -12;
	scale[1][11][1][(6+Tone)%12] = -12;
	scale[1][11][1][(7+Tone)%12] = -12;
	scale[1][11][1][(8+Tone)%12] = -12;
	scale[1][11][1][(9+Tone)%12] = -12;
	scale[1][11][1][(10+Tone)%12] = -12;
	scale[1][11][1][(11+Tone)%12] = -12;
	scale[1][11][2][(0+Tone)%12] = -12;
	scale[1][11][2][(1+Tone)%12] = -12;
	scale[1][11][2][(2+Tone)%12] = -12;
	scale[1][11][2][(3+Tone)%12] = -12;
	scale[1][11][2][(4+Tone)%12] = -12;
	scale[1][11][2][(5+Tone)%12] = -12;
	scale[1][11][2][(6+Tone)%12] = -12;
	scale[1][11][2][(7+Tone)%12] = -12;
	scale[1][11][2][(8+Tone)%12] = -12;
	scale[1][11][2][(9+Tone)%12] = -12;
	scale[1][11][2][(10+Tone)%12] = -12;
	scale[1][11][2][(11+Tone)%12] = -12;

	
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
	
	//Quarta acima

	scale[2][1][0][(0+Tone)%12] = 5;
	scale[2][1][0][(1+Tone)%12] = 5;
	scale[2][1][0][(2+Tone)%12] = 5;
	scale[2][1][0][(3+Tone)%12] = 5;
	scale[2][1][0][(4+Tone)%12] = 5;
	scale[2][1][0][(5+Tone)%12] = 6;
	scale[2][1][0][(6+Tone)%12] = 5.5;
	scale[2][1][0][(7+Tone)%12] = 5;
	scale[2][1][0][(8+Tone)%12] = 5;
	scale[2][1][0][(9+Tone)%12] = 5;
	scale[2][1][0][(10+Tone)%12] = 5;
	scale[2][1][0][(11+Tone)%12] = 5;
	scale[2][1][1][(0+Tone)%12] = 5;
	scale[2][1][1][(1+Tone)%12] = 5;
	scale[2][1][1][(2+Tone)%12] = 5;
	scale[2][1][1][(3+Tone)%12] = 5;
	scale[2][1][1][(4+Tone)%12] = 5;
	scale[2][1][1][(5+Tone)%12] = 5;
	scale[2][1][1][(6+Tone)%12] = 5;
	scale[2][1][1][(7+Tone)%12] = 5;
	scale[2][1][1][(8+Tone)%12] = 6;
	scale[2][1][1][(9+Tone)%12] = 5.5;
	scale[2][1][1][(10+Tone)%12] = 5;
	scale[2][1][1][(11+Tone)%12] = 5;
	scale[2][1][2][(0+Tone)%12] = 5;
	scale[2][1][2][(1+Tone)%12] = 5;
	scale[2][1][2][(2+Tone)%12] = 5;
	scale[2][1][2][(3+Tone)%12] = 5;
	scale[2][1][2][(4+Tone)%12] = 5.5;
	scale[2][1][2][(5+Tone)%12] = 6;
	scale[2][1][2][(6+Tone)%12] = 5.5;
	scale[2][1][2][(7+Tone)%12] = 5;
	scale[2][1][2][(8+Tone)%12] = 6;
	scale[2][1][2][(9+Tone)%12] = 5.33;
	scale[2][1][2][(10+Tone)%12] = 4.66;
	scale[2][1][2][(11+Tone)%12] = 4;

	//Quinta acima

	scale[2][2][0][(0+Tone)%12] = 7;
	scale[2][2][0][(1+Tone)%12] = 7;
	scale[2][2][0][(2+Tone)%12] = 7;
	scale[2][2][0][(3+Tone)%12] = 7;
	scale[2][2][0][(4+Tone)%12] = 7;
	scale[2][2][0][(5+Tone)%12] = 7;
	scale[2][2][0][(6+Tone)%12] = 7;
	scale[2][2][0][(7+Tone)%12] = 7;
	scale[2][2][0][(8+Tone)%12] = 7;
	scale[2][2][0][(9+Tone)%12] = 7;
	scale[2][2][0][(10+Tone)%12] = 6.5;
	scale[2][2][0][(11+Tone)%12] = 6;
	scale[2][2][1][(0+Tone)%12] = 7;
	scale[2][2][1][(1+Tone)%12] = 6.5;
	scale[2][2][1][(2+Tone)%12] = 6;
	scale[2][2][1][(3+Tone)%12] = 7;
	scale[2][2][1][(4+Tone)%12] = 7;
	scale[2][2][1][(5+Tone)%12] = 7;
	scale[2][2][1][(6+Tone)%12] = 7;
	scale[2][2][1][(7+Tone)%12] = 7;
	scale[2][2][1][(8+Tone)%12] = 7;
	scale[2][2][1][(9+Tone)%12] = 7;
	scale[2][2][1][(10+Tone)%12] = 7;
	scale[2][2][1][(11+Tone)%12] = 7;
	scale[2][2][2][(0+Tone)%12] = 7;
	scale[2][2][2][(1+Tone)%12] = 6.5;
	scale[2][2][2][(2+Tone)%12] = 6;
	scale[2][2][2][(3+Tone)%12] = 8;
	scale[2][2][2][(4+Tone)%12] = 7.5;
	scale[2][2][2][(5+Tone)%12] = 7;
	scale[2][2][2][(6+Tone)%12] = 7;
	scale[2][2][2][(7+Tone)%12] = 7;
	scale[2][2][2][(8+Tone)%12] = 7;
	scale[2][2][2][(9+Tone)%12] = 6.66;
	scale[2][2][2][(10+Tone)%12] = 6.33;
	scale[2][2][2][(11+Tone)%12] = 6;

	//Sexta acima

	scale[2][3][0][(0+Tone)%12] = 9;
	scale[2][3][0][(1+Tone)%12] = 9;
	scale[2][3][0][(2+Tone)%12] = 9;
	scale[2][3][0][(3+Tone)%12] = 8.5;
	scale[2][3][0][(4+Tone)%12] = 8;
	scale[2][3][0][(5+Tone)%12] = 9;
	scale[2][3][0][(6+Tone)%12] = 9;
	scale[2][3][0][(7+Tone)%12] = 9;
	scale[2][3][0][(8+Tone)%12] = 8.5;
	scale[2][3][0][(9+Tone)%12] = 8;
	scale[2][3][0][(10+Tone)%12] = 8;
	scale[2][3][0][(11+Tone)%12] = 8;
	scale[2][3][1][(0+Tone)%12] = 8;
	scale[2][3][1][(1+Tone)%12] = 8;
	scale[2][3][1][(2+Tone)%12] = 8;
	scale[2][3][1][(3+Tone)%12] = 9;
	scale[2][3][1][(4+Tone)%12] = 9;
	scale[2][3][1][(5+Tone)%12] = 9;
	scale[2][3][1][(6+Tone)%12] = 8.5;
	scale[2][3][1][(7+Tone)%12] = 8;
	scale[2][3][1][(8+Tone)%12] = 9;
	scale[2][3][1][(9+Tone)%12] = 9;
	scale[2][3][1][(10+Tone)%12] = 9;
	scale[2][3][1][(11+Tone)%12] = 8.5;
	scale[2][3][2][(0+Tone)%12] = 8;
	scale[2][3][2][(1+Tone)%12] = 8.5;
	scale[2][3][2][(2+Tone)%12] = 9;
	scale[2][3][2][(3+Tone)%12] = 9;
	scale[2][3][2][(4+Tone)%12] = 9;
	scale[2][3][2][(5+Tone)%12] = 9;
	scale[2][3][2][(6+Tone)%12] = 8.5;
	scale[2][3][2][(7+Tone)%12] = 8;
	scale[2][3][2][(8+Tone)%12] = 9;
	scale[2][3][2][(9+Tone)%12] = 8.66;
	scale[2][3][2][(10+Tone)%12] = 8.33;
	scale[2][3][2][(11+Tone)%12] = 8;
	
	//Sétima acima

	scale[2][4][0][(0+Tone)%12] = 11;
	scale[2][4][0][(1+Tone)%12] = 10.5;
	scale[2][4][0][(2+Tone)%12] = 10;
	scale[2][4][0][(3+Tone)%12] = 10;
	scale[2][4][0][(4+Tone)%12] = 10;
	scale[2][4][0][(5+Tone)%12] = 11;
	scale[2][4][0][(6+Tone)%12] = 10.5;
	scale[2][4][0][(7+Tone)%12] = 10;
	scale[2][4][0][(8+Tone)%12] = 10;
	scale[2][4][0][(9+Tone)%12] = 10;
	scale[2][4][0][(10+Tone)%12] = 10;
	scale[2][4][0][(11+Tone)%12] = 10;
	scale[2][4][1][(0+Tone)%12] = 10;
	scale[2][4][1][(1+Tone)%12] = 10;
	scale[2][4][1][(2+Tone)%12] = 10;
	scale[2][4][1][(3+Tone)%12] = 11;
	scale[2][4][1][(4+Tone)%12] = 10.5;
	scale[2][4][1][(5+Tone)%12] = 10;
	scale[2][4][1][(6+Tone)%12] = 10;
	scale[2][4][1][(7+Tone)%12] = 10;
	scale[2][4][1][(8+Tone)%12] = 11;
	scale[2][4][1][(9+Tone)%12] = 10.5;
	scale[2][4][1][(10+Tone)%12] = 10;
	scale[2][4][1][(11+Tone)%12] = 10;
	scale[2][4][2][(0+Tone)%12] = 11;
	scale[2][4][2][(1+Tone)%12] = 10.5;
	scale[2][4][2][(2+Tone)%12] = 10;
	scale[2][4][2][(3+Tone)%12] = 11;
	scale[2][4][2][(4+Tone)%12] = 10.5;
	scale[2][4][2][(5+Tone)%12] = 10;
	scale[2][4][2][(6+Tone)%12] = 10;
	scale[2][4][2][(7+Tone)%12] = 10;
	scale[2][4][2][(8+Tone)%12] = 11;
	scale[2][4][2][(9+Tone)%12] = 10.33;
	scale[2][4][2][(10+Tone)%12] = 9.66;
	scale[2][4][2][(11+Tone)%12] = 9;

	//Oitava acima

	scale[2][5][0][(0+Tone)%12] = 12;
	scale[2][5][0][(1+Tone)%12] = 12;
	scale[2][5][0][(2+Tone)%12] = 12;
	scale[2][5][0][(3+Tone)%12] = 12;
	scale[2][5][0][(4+Tone)%12] = 12;
	scale[2][5][0][(5+Tone)%12] = 12;
	scale[2][5][0][(6+Tone)%12] = 12;
	scale[2][5][0][(7+Tone)%12] = 12;
	scale[2][5][0][(8+Tone)%12] = 12;
	scale[2][5][0][(9+Tone)%12] = 12;
	scale[2][5][0][(10+Tone)%12] = 12;
	scale[2][5][0][(11+Tone)%12] = 12;
	scale[2][5][1][(0+Tone)%12] = 12;
	scale[2][5][1][(1+Tone)%12] = 12;
	scale[2][5][1][(2+Tone)%12] = 12;
	scale[2][5][1][(3+Tone)%12] = 12;
	scale[2][5][1][(4+Tone)%12] = 12;
	scale[2][5][1][(5+Tone)%12] = 12;
	scale[2][5][1][(6+Tone)%12] = 12;
	scale[2][5][1][(7+Tone)%12] = 12;
	scale[2][5][1][(8+Tone)%12] = 12;
	scale[2][5][1][(9+Tone)%12] = 12;
	scale[2][5][1][(10+Tone)%12] = 12;
	scale[2][5][1][(11+Tone)%12] = 12;
	scale[2][5][2][(0+Tone)%12] = 12;
	scale[2][5][2][(1+Tone)%12] = 12;
	scale[2][5][2][(2+Tone)%12] = 12;
	scale[2][5][2][(3+Tone)%12] = 12;
	scale[2][5][2][(4+Tone)%12] = 12;
	scale[2][5][2][(5+Tone)%12] = 12;
	scale[2][5][2][(6+Tone)%12] = 12;
	scale[2][5][2][(7+Tone)%12] = 12;
	scale[2][5][2][(8+Tone)%12] = 12;
	scale[2][5][2][(9+Tone)%12] = 12;
	scale[2][5][2][(10+Tone)%12] = 12;
	scale[2][5][2][(11+Tone)%12] = 12;

	//Terça abaixo

	scale[2][6][0][(0+Tone)%12] = -3;
	scale[2][6][0][(1+Tone)%12] = -3;
	scale[2][6][0][(2+Tone)%12] = -3;
	scale[2][6][0][(3+Tone)%12] = -3.5;
	scale[2][6][0][(4+Tone)%12] = -4;
	scale[2][6][0][(5+Tone)%12] = -3;
	scale[2][6][0][(6+Tone)%12] = -3;
	scale[2][6][0][(7+Tone)%12] = -3;
	scale[2][6][0][(8+Tone)%12] = -3.5;
	scale[2][6][0][(9+Tone)%12] = -4;
	scale[2][6][0][(10+Tone)%12] = -4;
	scale[2][6][0][(11+Tone)%12] = -4;
	scale[2][6][1][(0+Tone)%12] = -4;
	scale[2][6][1][(1+Tone)%12] = -4;
	scale[2][6][1][(2+Tone)%12] = -4;
	scale[2][6][1][(3+Tone)%12] = -3;
	scale[2][6][1][(4+Tone)%12] = -3;
	scale[2][6][1][(5+Tone)%12] = -3;
	scale[2][6][1][(6+Tone)%12] = -3.5;
	scale[2][6][1][(7+Tone)%12] = -4;
	scale[2][6][1][(8+Tone)%12] = -3;
	scale[2][6][1][(9+Tone)%12] = -3;
	scale[2][6][1][(10+Tone)%12] = -3;
	scale[2][6][1][(11+Tone)%12] = -3.5;
	scale[2][6][2][(0+Tone)%12] = -4;
	scale[2][6][2][(1+Tone)%12] = -3.5;
	scale[2][6][2][(2+Tone)%12] = -3;
	scale[2][6][2][(3+Tone)%12] = -3;
	scale[2][6][2][(4+Tone)%12] = -3;
	scale[2][6][2][(5+Tone)%12] = -3;
	scale[2][6][2][(6+Tone)%12] = -3.5;
	scale[2][6][2][(7+Tone)%12] = -4;
	scale[2][6][2][(8+Tone)%12] = -3;
	scale[2][6][2][(9+Tone)%12] = -3.33;
	scale[2][6][2][(10+Tone)%12] = -3.66;
	scale[2][6][2][(11+Tone)%12] = -4;
	
	//Quarta abaixo

	scale[2][7][0][(0+Tone)%12] = -5;
	scale[2][7][0][(1+Tone)%12] = -5;
	scale[2][7][0][(2+Tone)%12] = -5;
	scale[2][7][0][(3+Tone)%12] = -5;
	scale[2][7][0][(4+Tone)%12] = -5;
	scale[2][7][0][(5+Tone)%12] = -5;
	scale[2][7][0][(6+Tone)%12] = -5;
	scale[2][7][0][(7+Tone)%12] = -5;
	scale[2][7][0][(8+Tone)%12] = -5;
	scale[2][7][0][(9+Tone)%12] = -5;
	scale[2][7][0][(10+Tone)%12] = -5.5;
	scale[2][7][0][(11+Tone)%12] = -6;
	scale[2][7][1][(0+Tone)%12] = -5;
	scale[2][7][1][(1+Tone)%12] = -5.5;
	scale[2][7][1][(2+Tone)%12] = -6;
	scale[2][7][1][(3+Tone)%12] = -5;
	scale[2][7][1][(4+Tone)%12] = -5;
	scale[2][7][1][(5+Tone)%12] = -5;
	scale[2][7][1][(6+Tone)%12] = -5;
	scale[2][7][1][(7+Tone)%12] = -5;
	scale[2][7][1][(8+Tone)%12] = -5;
	scale[2][7][1][(9+Tone)%12] = -5;
	scale[2][7][1][(10+Tone)%12] = -5;
	scale[2][7][1][(11+Tone)%12] = -5;
	scale[2][7][2][(0+Tone)%12] = -5;
	scale[2][7][2][(1+Tone)%12] = -5.5;
	scale[2][7][2][(2+Tone)%12] = -6;
	scale[2][7][2][(3+Tone)%12] = -4;
	scale[2][7][2][(4+Tone)%12] = -4.5;
	scale[2][7][2][(5+Tone)%12] = -5;
	scale[2][7][2][(6+Tone)%12] = -5;
	scale[2][7][2][(7+Tone)%12] = -5;
	scale[2][7][2][(8+Tone)%12] = -5;
	scale[2][7][2][(9+Tone)%12] = -5.33;
	scale[2][7][2][(10+Tone)%12] = -5.66;
	scale[2][7][2][(11+Tone)%12] = -6;

	//Quinta abaixo

	scale[2][8][0][(0+Tone)%12] = -7;
	scale[2][8][0][(1+Tone)%12] = -7;
	scale[2][8][0][(2+Tone)%12] = -7;
	scale[2][8][0][(3+Tone)%12] = -7;
	scale[2][8][0][(4+Tone)%12] = -7;
	scale[2][8][0][(5+Tone)%12] = -6;
	scale[2][8][0][(6+Tone)%12] = -6.5;
	scale[2][8][0][(7+Tone)%12] = -7;
	scale[2][8][0][(8+Tone)%12] = -7;
	scale[2][8][0][(9+Tone)%12] = -7;
	scale[2][8][0][(10+Tone)%12] = -7;
	scale[2][8][0][(11+Tone)%12] = -7;
	scale[2][8][1][(0+Tone)%12] = -7;
	scale[2][8][1][(1+Tone)%12] = -7;
	scale[2][8][1][(2+Tone)%12] = -7;
	scale[2][8][1][(3+Tone)%12] = -7;
	scale[2][8][1][(4+Tone)%12] = -7;
	scale[2][8][1][(5+Tone)%12] = -7;
	scale[2][8][1][(6+Tone)%12] = -7;
	scale[2][8][1][(7+Tone)%12] = -7;
	scale[2][8][1][(8+Tone)%12] = -6;
	scale[2][8][1][(9+Tone)%12] = -6.5;
	scale[2][8][1][(10+Tone)%12] = -7;
	scale[2][8][1][(11+Tone)%12] = -7;
	scale[2][8][2][(0+Tone)%12] = -7;
	scale[2][8][2][(1+Tone)%12] = -7;
	scale[2][8][2][(2+Tone)%12] = -7;
	scale[2][8][2][(3+Tone)%12] = -7;
	scale[2][8][2][(4+Tone)%12] = -6.5;
	scale[2][8][2][(5+Tone)%12] = -6;
	scale[2][8][2][(6+Tone)%12] = -6.5;
	scale[2][8][2][(7+Tone)%12] = -7;
	scale[2][8][2][(8+Tone)%12] = -6;
	scale[2][8][2][(9+Tone)%12] = -6.66;
	scale[2][8][2][(10+Tone)%12] = -7.33;
	scale[2][8][2][(11+Tone)%12] = -8;

	//Sexta abaixo

	scale[2][9][0][(0+Tone)%12] = -8;
	scale[2][9][0][(1+Tone)%12] = -8.5;
	scale[2][9][0][(2+Tone)%12] = -9;
	scale[2][9][0][(3+Tone)%12] = -9;
	scale[2][9][0][(4+Tone)%12] = -9;
	scale[2][9][0][(5+Tone)%12] = -8;
	scale[2][9][0][(6+Tone)%12] = -8;
	scale[2][9][0][(7+Tone)%12] = -8;
	scale[2][9][0][(8+Tone)%12] = -8.5;
	scale[2][9][0][(9+Tone)%12] = -9;
	scale[2][9][0][(10+Tone)%12] = -9;
	scale[2][9][0][(11+Tone)%12] = -9;	
	scale[2][9][1][(0+Tone)%12] = -9;
	scale[2][9][1][(1+Tone)%12] = -9;
	scale[2][9][1][(2+Tone)%12] = -9;
	scale[2][9][1][(3+Tone)%12] = -8;
	scale[2][9][1][(4+Tone)%12] = -8.5;
	scale[2][9][1][(5+Tone)%12] = -9;
	scale[2][9][1][(6+Tone)%12] = -9;
	scale[2][9][1][(7+Tone)%12] = -9;
	scale[2][9][1][(8+Tone)%12] = -8;
	scale[2][9][1][(9+Tone)%12] = -8;
	scale[2][9][1][(10+Tone)%12] = -8;
	scale[2][9][1][(11+Tone)%12] = -8.5;
	scale[2][9][2][(0+Tone)%12] = -9;
	scale[2][9][2][(1+Tone)%12] = -9;
	scale[2][9][2][(2+Tone)%12] = -9;
	scale[2][9][2][(3+Tone)%12] = -8;
	scale[2][9][2][(4+Tone)%12] = -8.5;
	scale[2][9][2][(5+Tone)%12] = -9;
	scale[2][9][2][(6+Tone)%12] = -9;
	scale[2][9][2][(7+Tone)%12] = -9;
	scale[2][9][2][(8+Tone)%12] = -8;
	scale[2][9][2][(9+Tone)%12] = -8.33;
	scale[2][9][2][(10+Tone)%12] = -8.66;
	scale[2][9][2][(11+Tone)%12] = -9;

	//Sétima abaixo

	scale[2][10][0][(0+Tone)%12] = -10;
	scale[2][10][0][(1+Tone)%12] = -10;
	scale[2][10][0][(2+Tone)%12] = -10;
	scale[2][10][0][(3+Tone)%12] = -10.5;
	scale[2][10][0][(4+Tone)%12] = -11;
	scale[2][10][0][(5+Tone)%12] = -10;
	scale[2][10][0][(6+Tone)%12] = -10;
	scale[2][10][0][(7+Tone)%12] = -10;
	scale[2][10][0][(8+Tone)%12] = -10;
	scale[2][10][0][(9+Tone)%12] = -10;
	scale[2][10][0][(10+Tone)%12] = -10.5;
	scale[2][10][0][(11+Tone)%12] = -11;
	scale[2][10][1][(0+Tone)%12] = -10;
	scale[2][10][1][(1+Tone)%12] = -10.5;
	scale[2][10][1][(2+Tone)%12] = -11;
	scale[2][10][1][(3+Tone)%12] = -10;
	scale[2][10][1][(4+Tone)%12] = -10;
	scale[2][10][1][(5+Tone)%12] = -10;
	scale[2][10][1][(6+Tone)%12] = -10.5;
	scale[2][10][1][(7+Tone)%12] = -11;
	scale[2][10][1][(8+Tone)%12] = -10;
	scale[2][10][1][(9+Tone)%12] = -10;
	scale[2][10][1][(10+Tone)%12] = -10;
	scale[2][10][1][(11+Tone)%12] = -10;
	scale[2][10][2][(0+Tone)%12] = -10;
	scale[2][10][2][(1+Tone)%12] = -10.5;
	scale[2][10][2][(2+Tone)%12] = -11;
	scale[2][10][2][(3+Tone)%12] = -10;
	scale[2][10][2][(4+Tone)%12] = -10;
	scale[2][10][2][(5+Tone)%12] = -10;
	scale[2][10][2][(6+Tone)%12] = -10.5;
	scale[2][10][2][(7+Tone)%12] = -11;
	scale[2][10][2][(8+Tone)%12] = -9;
	scale[2][10][2][(9+Tone)%12] = -9.66;
	scale[2][10][2][(10+Tone)%12] = -10.33;
	scale[2][10][2][(11+Tone)%12] = -11;

	//Oitava abaixo

	scale[2][11][0][(0+Tone)%12] = -12;
	scale[2][11][0][(1+Tone)%12] = -12;
	scale[2][11][0][(2+Tone)%12] = -12;
	scale[2][11][0][(3+Tone)%12] = -12;
	scale[2][11][0][(4+Tone)%12] = -12;
	scale[2][11][0][(5+Tone)%12] = -12;
	scale[2][11][0][(6+Tone)%12] = -12;
	scale[2][11][0][(7+Tone)%12] = -12;
	scale[2][11][0][(8+Tone)%12] = -12;
	scale[2][11][0][(9+Tone)%12] = -12;
	scale[2][11][0][(10+Tone)%12] = -12;
	scale[2][11][0][(11+Tone)%12] = -12;
	scale[2][11][1][(0+Tone)%12] = -12;
	scale[2][11][1][(1+Tone)%12] = -12;
	scale[2][11][1][(2+Tone)%12] = -12;
	scale[2][11][1][(3+Tone)%12] = -12;
	scale[2][11][1][(4+Tone)%12] = -12;
	scale[2][11][1][(5+Tone)%12] = -12;
	scale[2][11][1][(6+Tone)%12] = -12;
	scale[2][11][1][(7+Tone)%12] = -12;
	scale[2][11][1][(8+Tone)%12] = -12;
	scale[2][11][1][(9+Tone)%12] = -12;
	scale[2][11][1][(10+Tone)%12] = -12;
	scale[2][11][1][(11+Tone)%12] = -12;
	scale[2][11][2][(0+Tone)%12] = -12;
	scale[2][11][2][(1+Tone)%12] = -12;
	scale[2][11][2][(2+Tone)%12] = -12;
	scale[2][11][2][(3+Tone)%12] = -12;
	scale[2][11][2][(4+Tone)%12] = -12;
	scale[2][11][2][(5+Tone)%12] = -12;
	scale[2][11][2][(6+Tone)%12] = -12;
	scale[2][11][2][(7+Tone)%12] = -12;
	scale[2][11][2][(8+Tone)%12] = -12;
	scale[2][11][2][(9+Tone)%12] = -12;
	scale[2][11][2][(10+Tone)%12] = -12;
	scale[2][11][2][(11+Tone)%12] = -12;

	
	if (Mode < 0) Mode = 0;
	if (Mode > 2) Mode = 2;
	if (Scale < 0) Scale = 0;
	if (Scale > 2) Scale = 2;
	if (Interval < 0) Interval = 0;
	if (Interval > 11) Interval = 11;
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
	//hops[Qcolumn-1] = round(hopa*(pow(2,(s[0]/12))));
	
}

void FindStepCS2(int note, int oitava, int Tone, int LowNote, int s_0, int s_1, int s_2, int s_3, int s_4, int s_5, int s_6, int s_7, int s_8, int s_9, int s_10, int s_11, double *s)
{
	float scale[12];	

	scale[(0+Tone)%12] = s_0;
	scale[(1+Tone)%12] = s_1;
	scale[(2+Tone)%12] = s_2;
	scale[(3+Tone)%12] = s_3;
	scale[(4+Tone)%12] = s_4;
	scale[(5+Tone)%12] = s_5;
	scale[(6+Tone)%12] = s_6;
	scale[(7+Tone)%12] = s_7;
	scale[(8+Tone)%12] = s_8;
	scale[(9+Tone)%12] = s_9;
	scale[(10+Tone)%12] = s_10;
	scale[(11+Tone)%12] = s_11;
	
	if (LowNote < 0) LowNote = 0;
	if (LowNote > 14) LowNote = 14;
	
	switch (note)
	{
		case 0:
			s[0] = scale[0];
			break;
		case 1:
			s[0] = scale[1];
			break;
		case 2:
			s[0] = scale[2];
			break;
		case 3:
			s[0] = scale[3];
			break;
		case 4:
			s[0] = scale[4];
			break;
		case 5:
			s[0] = scale[5];
			break;
		case 6:
			s[0] = scale[6];
			break;
		case 7:
			s[0] = scale[7];
			break;
		case 8:
			s[0] = scale[8];
			break;
		case 9:
			s[0] = scale[9];
			break;
		case 10:
			s[0] = scale[10];
			break;
		case 11:
			s[0] = scale[11];
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
	//hops[Qcolumn-1] = round(hopa*(pow(2,(s[0]/12))));	
}

void FindStep(int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, int hopa, int Qcolumn, double *s, int *hops)
{
	float scale[3][12][3][12];
		
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
	
	//Quarta acima

	scale[0][1][0][(0+Tone)%12] = 5;
	scale[0][1][0][(1+Tone)%12] = 0;
	scale[0][1][0][(2+Tone)%12] = 5;
	scale[0][1][0][(3+Tone)%12] = 0;
	scale[0][1][0][(4+Tone)%12] = 5;
	scale[0][1][0][(5+Tone)%12] = 6;
	scale[0][1][0][(6+Tone)%12] = 0;
	scale[0][1][0][(7+Tone)%12] = 5;
	scale[0][1][0][(8+Tone)%12] = 0;
	scale[0][1][0][(9+Tone)%12] = 5;
	scale[0][1][0][(10+Tone)%12] = 0;
	scale[0][1][0][(11+Tone)%12] = 5;
	scale[0][1][1][(0+Tone)%12] = 5;
	scale[0][1][1][(1+Tone)%12] = 0;
	scale[0][1][1][(2+Tone)%12] = 5;
	scale[0][1][1][(3+Tone)%12] = 5;
	scale[0][1][1][(4+Tone)%12] = 0;
	scale[0][1][1][(5+Tone)%12] = 5;
	scale[0][1][1][(6+Tone)%12] = 0;
	scale[0][1][1][(7+Tone)%12] = 5;
	scale[0][1][1][(8+Tone)%12] = 6;
	scale[0][1][1][(9+Tone)%12] = 0;
	scale[0][1][1][(10+Tone)%12] = 5;
	scale[0][1][1][(11+Tone)%12] = 0;
	scale[0][1][2][(0+Tone)%12] = 5;
	scale[0][1][2][(1+Tone)%12] = 0;
	scale[0][1][2][(2+Tone)%12] = 5;
	scale[0][1][2][(3+Tone)%12] = 5;
	scale[0][1][2][(4+Tone)%12] = 0;
	scale[0][1][2][(5+Tone)%12] = 6;
	scale[0][1][2][(6+Tone)%12] = 0;
	scale[0][1][2][(7+Tone)%12] = 5;
	scale[0][1][2][(8+Tone)%12] = 6;
	scale[0][1][2][(9+Tone)%12] = 0;
	scale[0][1][2][(10+Tone)%12] = 0;
	scale[0][1][2][(11+Tone)%12] = 4;

	//Quinta acima

	scale[0][2][0][(0+Tone)%12] = 7;
	scale[0][2][0][(1+Tone)%12] = 0;
	scale[0][2][0][(2+Tone)%12] = 7;
	scale[0][2][0][(3+Tone)%12] = 0;
	scale[0][2][0][(4+Tone)%12] = 7;
	scale[0][2][0][(5+Tone)%12] = 7;
	scale[0][2][0][(6+Tone)%12] = 0;
	scale[0][2][0][(7+Tone)%12] = 7;
	scale[0][2][0][(8+Tone)%12] = 0;
	scale[0][2][0][(9+Tone)%12] = 7;
	scale[0][2][0][(10+Tone)%12] = 0;
	scale[0][2][0][(11+Tone)%12] = 6;
	scale[0][2][1][(0+Tone)%12] = 7;
	scale[0][2][1][(1+Tone)%12] = 0;
	scale[0][2][1][(2+Tone)%12] = 6;
	scale[0][2][1][(3+Tone)%12] = 7;
	scale[0][2][1][(4+Tone)%12] = 0;
	scale[0][2][1][(5+Tone)%12] = 7;
	scale[0][2][1][(6+Tone)%12] = 0;
	scale[0][2][1][(7+Tone)%12] = 7;
	scale[0][2][1][(8+Tone)%12] = 7;
	scale[0][2][1][(9+Tone)%12] = 0;
	scale[0][2][1][(10+Tone)%12] = 7;
	scale[0][2][1][(11+Tone)%12] = 0;
	scale[0][2][2][(0+Tone)%12] = 7;
	scale[0][2][2][(1+Tone)%12] = 0;
	scale[0][2][2][(2+Tone)%12] = 6;
	scale[0][2][2][(3+Tone)%12] = 8;
	scale[0][2][2][(4+Tone)%12] = 0;
	scale[0][2][2][(5+Tone)%12] = 7;
	scale[0][2][2][(6+Tone)%12] = 0;
	scale[0][2][2][(7+Tone)%12] = 7;
	scale[0][2][2][(8+Tone)%12] = 7;
	scale[0][2][2][(9+Tone)%12] = 0;
	scale[0][2][2][(10+Tone)%12] = 0;
	scale[0][2][2][(11+Tone)%12] = 6;

	//Sexta acima

	scale[0][3][0][(0+Tone)%12] = 9;
	scale[0][3][0][(1+Tone)%12] = 0;
	scale[0][3][0][(2+Tone)%12] = 9;
	scale[0][3][0][(3+Tone)%12] = 0;
	scale[0][3][0][(4+Tone)%12] = 8;
	scale[0][3][0][(5+Tone)%12] = 9;
	scale[0][3][0][(6+Tone)%12] = 0;
	scale[0][3][0][(7+Tone)%12] = 9;
	scale[0][3][0][(8+Tone)%12] = 0;
	scale[0][3][0][(9+Tone)%12] = 8;
	scale[0][3][0][(10+Tone)%12] = 0;
	scale[0][3][0][(11+Tone)%12] = 8;
	scale[0][3][1][(0+Tone)%12] = 8;
	scale[0][3][1][(1+Tone)%12] = 0;
	scale[0][3][1][(2+Tone)%12] = 8;
	scale[0][3][1][(3+Tone)%12] = 9;
	scale[0][3][1][(4+Tone)%12] = 0;
	scale[0][3][1][(5+Tone)%12] = 9;
	scale[0][3][1][(6+Tone)%12] = 0;
	scale[0][3][1][(7+Tone)%12] = 8;
	scale[0][3][1][(8+Tone)%12] = 9;
	scale[0][3][1][(9+Tone)%12] = 0;
	scale[0][3][1][(10+Tone)%12] = 9;
	scale[0][3][1][(11+Tone)%12] = 0;
	scale[0][3][2][(0+Tone)%12] = 8;
	scale[0][3][2][(1+Tone)%12] = 0;
	scale[0][3][2][(2+Tone)%12] = 9;
	scale[0][3][2][(3+Tone)%12] = 9;
	scale[0][3][2][(4+Tone)%12] = 0;
	scale[0][3][2][(5+Tone)%12] = 9;
	scale[0][3][2][(6+Tone)%12] = 0;
	scale[0][3][2][(7+Tone)%12] = 8;
	scale[0][3][2][(8+Tone)%12] = 9;
	scale[0][3][2][(9+Tone)%12] = 0;
	scale[0][3][2][(10+Tone)%12] = 0;
	scale[0][3][2][(11+Tone)%12] = 8;
	
	//Sétima acima

	scale[0][4][0][(0+Tone)%12] = 11;
	scale[0][4][0][(1+Tone)%12] = 0;
	scale[0][4][0][(2+Tone)%12] = 10;
	scale[0][4][0][(3+Tone)%12] = 0;
	scale[0][4][0][(4+Tone)%12] = 10;
	scale[0][4][0][(5+Tone)%12] = 11;
	scale[0][4][0][(6+Tone)%12] = 0;
	scale[0][4][0][(7+Tone)%12] = 10;
	scale[0][4][0][(8+Tone)%12] = 0;
	scale[0][4][0][(9+Tone)%12] = 10;
	scale[0][4][0][(10+Tone)%12] = 0;
	scale[0][4][0][(11+Tone)%12] = 10;
	scale[0][4][1][(0+Tone)%12] = 10;
	scale[0][4][1][(1+Tone)%12] = 0;
	scale[0][4][1][(2+Tone)%12] = 10;
	scale[0][4][1][(3+Tone)%12] = 11;
	scale[0][4][1][(4+Tone)%12] = 0;
	scale[0][4][1][(5+Tone)%12] = 10;
	scale[0][4][1][(6+Tone)%12] = 0;
	scale[0][4][1][(7+Tone)%12] = 10;
	scale[0][4][1][(8+Tone)%12] = 11;
	scale[0][4][1][(9+Tone)%12] = 0;
	scale[0][4][1][(10+Tone)%12] = 10;
	scale[0][4][1][(11+Tone)%12] = 0;
	scale[0][4][2][(0+Tone)%12] = 11;
	scale[0][4][2][(1+Tone)%12] = 0;
	scale[0][4][2][(2+Tone)%12] = 10;
	scale[0][4][2][(3+Tone)%12] = 11;
	scale[0][4][2][(4+Tone)%12] = 0;
	scale[0][4][2][(5+Tone)%12] = 10;
	scale[0][4][2][(6+Tone)%12] = 0;
	scale[0][4][2][(7+Tone)%12] = 10;
	scale[0][4][2][(8+Tone)%12] = 11;
	scale[0][4][2][(9+Tone)%12] = 0;
	scale[0][4][2][(10+Tone)%12] = 0;
	scale[0][4][2][(11+Tone)%12] = 9;

	//Oitava acima

	scale[0][5][0][(0+Tone)%12] = 12;
	scale[0][5][0][(1+Tone)%12] = 0;
	scale[0][5][0][(2+Tone)%12] = 12;
	scale[0][5][0][(3+Tone)%12] = 0;
	scale[0][5][0][(4+Tone)%12] = 12;
	scale[0][5][0][(5+Tone)%12] = 12;
	scale[0][5][0][(6+Tone)%12] = 0;
	scale[0][5][0][(7+Tone)%12] = 12;
	scale[0][5][0][(8+Tone)%12] = 0;
	scale[0][5][0][(9+Tone)%12] = 12;
	scale[0][5][0][(10+Tone)%12] = 0;
	scale[0][5][0][(11+Tone)%12] = 12;
	scale[0][5][1][(0+Tone)%12] = 12;
	scale[0][5][1][(1+Tone)%12] = 0;
	scale[0][5][1][(2+Tone)%12] = 12;
	scale[0][5][1][(3+Tone)%12] = 12;
	scale[0][5][1][(4+Tone)%12] = 0;
	scale[0][5][1][(5+Tone)%12] = 12;
	scale[0][5][1][(6+Tone)%12] = 0;
	scale[0][5][1][(7+Tone)%12] = 12;
	scale[0][5][1][(8+Tone)%12] = 12;
	scale[0][5][1][(9+Tone)%12] = 0;
	scale[0][5][1][(10+Tone)%12] = 12;
	scale[0][5][1][(11+Tone)%12] = 0;
	scale[0][5][2][(0+Tone)%12] = 12;
	scale[0][5][2][(1+Tone)%12] = 0;
	scale[0][5][2][(2+Tone)%12] = 12;
	scale[0][5][2][(3+Tone)%12] = 12;
	scale[0][5][2][(4+Tone)%12] = 0;
	scale[0][5][2][(5+Tone)%12] = 12;
	scale[0][5][2][(6+Tone)%12] = 0;
	scale[0][5][2][(7+Tone)%12] = 12;
	scale[0][5][2][(8+Tone)%12] = 12;
	scale[0][5][2][(9+Tone)%12] = 0;
	scale[0][5][2][(10+Tone)%12] = 0;
	scale[0][5][2][(11+Tone)%12] = 12;

	//Terça abaixo

	scale[0][6][0][(0+Tone)%12] = -3;
	scale[0][6][0][(1+Tone)%12] = 0;
	scale[0][6][0][(2+Tone)%12] = -3;
	scale[0][6][0][(3+Tone)%12] = 0;
	scale[0][6][0][(4+Tone)%12] = -4;
	scale[0][6][0][(5+Tone)%12] = -3;
	scale[0][6][0][(6+Tone)%12] = 0;
	scale[0][6][0][(7+Tone)%12] = -3;
	scale[0][6][0][(8+Tone)%12] = 0;
	scale[0][6][0][(9+Tone)%12] = -4;
	scale[0][6][0][(10+Tone)%12] = 0;
	scale[0][6][0][(11+Tone)%12] = -4;
	scale[0][6][1][(0+Tone)%12] = -4;
	scale[0][6][1][(1+Tone)%12] = 0;
	scale[0][6][1][(2+Tone)%12] = -4;
	scale[0][6][1][(3+Tone)%12] = -3;
	scale[0][6][1][(4+Tone)%12] = 0;
	scale[0][6][1][(5+Tone)%12] = -3;
	scale[0][6][1][(6+Tone)%12] = 0;
	scale[0][6][1][(7+Tone)%12] = -4;
	scale[0][6][1][(8+Tone)%12] = -3;
	scale[0][6][1][(9+Tone)%12] = 0;
	scale[0][6][1][(10+Tone)%12] = -3;
	scale[0][6][1][(11+Tone)%12] = 0;
	scale[0][6][2][(0+Tone)%12] = -4;
	scale[0][6][2][(1+Tone)%12] = 0;
	scale[0][6][2][(2+Tone)%12] = -3;
	scale[0][6][2][(3+Tone)%12] = -3;
	scale[0][6][2][(4+Tone)%12] = 0;
	scale[0][6][2][(5+Tone)%12] = -3;
	scale[0][6][2][(6+Tone)%12] = 0;
	scale[0][6][2][(7+Tone)%12] = -4;
	scale[0][6][2][(8+Tone)%12] = -3;
	scale[0][6][2][(9+Tone)%12] = 0;
	scale[0][6][2][(10+Tone)%12] = 0;
	scale[0][6][2][(11+Tone)%12] = -4;
	
	//Quarta abaixo

	scale[0][7][0][(0+Tone)%12] = -5;
	scale[0][7][0][(1+Tone)%12] = 0;
	scale[0][7][0][(2+Tone)%12] = -5;
	scale[0][7][0][(3+Tone)%12] = 0;
	scale[0][7][0][(4+Tone)%12] = -5;
	scale[0][7][0][(5+Tone)%12] = -5;
	scale[0][7][0][(6+Tone)%12] = 0;
	scale[0][7][0][(7+Tone)%12] = -5;
	scale[0][7][0][(8+Tone)%12] = 0;
	scale[0][7][0][(9+Tone)%12] = -5;
	scale[0][7][0][(10+Tone)%12] = 0;
	scale[0][7][0][(11+Tone)%12] = -6;
	scale[0][7][1][(0+Tone)%12] = -5;
	scale[0][7][1][(1+Tone)%12] = 0;
	scale[0][7][1][(2+Tone)%12] = -6;
	scale[0][7][1][(3+Tone)%12] = -5;
	scale[0][7][1][(4+Tone)%12] = 0;
	scale[0][7][1][(5+Tone)%12] = -5;
	scale[0][7][1][(6+Tone)%12] = 0;
	scale[0][7][1][(7+Tone)%12] = -5;
	scale[0][7][1][(8+Tone)%12] = -5;
	scale[0][7][1][(9+Tone)%12] = 0;
	scale[0][7][1][(10+Tone)%12] = -5;
	scale[0][7][1][(11+Tone)%12] = 0;
	scale[0][7][2][(0+Tone)%12] = -5;
	scale[0][7][2][(1+Tone)%12] = 0;
	scale[0][7][2][(2+Tone)%12] = -6;
	scale[0][7][2][(3+Tone)%12] = -4;
	scale[0][7][2][(4+Tone)%12] = 0;
	scale[0][7][2][(5+Tone)%12] = -5;
	scale[0][7][2][(6+Tone)%12] = 0;
	scale[0][7][2][(7+Tone)%12] = -5;
	scale[0][7][2][(8+Tone)%12] = -5;
	scale[0][7][2][(9+Tone)%12] = 0;
	scale[0][7][2][(10+Tone)%12] = 0;
	scale[0][7][2][(11+Tone)%12] = -6;

	//Quinta abaixo

	scale[0][8][0][(0+Tone)%12] = -7;
	scale[0][8][0][(1+Tone)%12] = 0;
	scale[0][8][0][(2+Tone)%12] = -7;
	scale[0][8][0][(3+Tone)%12] = 0;
	scale[0][8][0][(4+Tone)%12] = -7;
	scale[0][8][0][(5+Tone)%12] = -6;
	scale[0][8][0][(6+Tone)%12] = 0;
	scale[0][8][0][(7+Tone)%12] = -7;
	scale[0][8][0][(8+Tone)%12] = 0;
	scale[0][8][0][(9+Tone)%12] = -7;
	scale[0][8][0][(10+Tone)%12] = 0;
	scale[0][8][0][(11+Tone)%12] = -7;
	scale[0][8][1][(0+Tone)%12] = -7;
	scale[0][8][1][(1+Tone)%12] = 0;
	scale[0][8][1][(2+Tone)%12] = -7;
	scale[0][8][1][(3+Tone)%12] = -7;
	scale[0][8][1][(4+Tone)%12] = 0;
	scale[0][8][1][(5+Tone)%12] = -7;
	scale[0][8][1][(6+Tone)%12] = 0;
	scale[0][8][1][(7+Tone)%12] = -7;
	scale[0][8][1][(8+Tone)%12] = -6;
	scale[0][8][1][(9+Tone)%12] = 0;
	scale[0][8][1][(10+Tone)%12] = -7;
	scale[0][8][1][(11+Tone)%12] = 0;
	scale[0][8][2][(0+Tone)%12] = -7;
	scale[0][8][2][(1+Tone)%12] = 0;
	scale[0][8][2][(2+Tone)%12] = -7;
	scale[0][8][2][(3+Tone)%12] = -7;
	scale[0][8][2][(4+Tone)%12] = 0;
	scale[0][8][2][(5+Tone)%12] = -6;
	scale[0][8][2][(6+Tone)%12] = 0;
	scale[0][8][2][(7+Tone)%12] = -7;
	scale[0][8][2][(8+Tone)%12] = -6;
	scale[0][8][2][(9+Tone)%12] = 0;
	scale[0][8][2][(10+Tone)%12] = 0;
	scale[0][8][2][(11+Tone)%12] = -8;

	//Sexta abaixo

	scale[0][9][0][(0+Tone)%12] = -8;
	scale[0][9][0][(1+Tone)%12] = 0;
	scale[0][9][0][(2+Tone)%12] = -9;
	scale[0][9][0][(3+Tone)%12] = 0;
	scale[0][9][0][(4+Tone)%12] = -9;
	scale[0][9][0][(5+Tone)%12] = -8;
	scale[0][9][0][(6+Tone)%12] = 0;
	scale[0][9][0][(7+Tone)%12] = -8;
	scale[0][9][0][(8+Tone)%12] = 0;
	scale[0][9][0][(9+Tone)%12] = -9;
	scale[0][9][0][(10+Tone)%12] = 0;
	scale[0][9][0][(11+Tone)%12] = -9;	
	scale[0][9][1][(0+Tone)%12] = -9;
	scale[0][9][1][(1+Tone)%12] = 0;
	scale[0][9][1][(2+Tone)%12] = -9;
	scale[0][9][1][(3+Tone)%12] = -8;
	scale[0][9][1][(4+Tone)%12] = 0;
	scale[0][9][1][(5+Tone)%12] = -9;
	scale[0][9][1][(6+Tone)%12] = 0;
	scale[0][9][1][(7+Tone)%12] = -9;
	scale[0][9][1][(8+Tone)%12] = -8;
	scale[0][9][1][(9+Tone)%12] = 0;
	scale[0][9][1][(10+Tone)%12] = -8;
	scale[0][9][1][(11+Tone)%12] = 0;
	scale[0][9][2][(0+Tone)%12] = -9;
	scale[0][9][2][(1+Tone)%12] = 0;
	scale[0][9][2][(2+Tone)%12] = -9;
	scale[0][9][2][(3+Tone)%12] = -8;
	scale[0][9][2][(4+Tone)%12] = 0;
	scale[0][9][2][(5+Tone)%12] = -9;
	scale[0][9][2][(6+Tone)%12] = 0;
	scale[0][9][2][(7+Tone)%12] = -9;
	scale[0][9][2][(8+Tone)%12] = -8;
	scale[0][9][2][(9+Tone)%12] = 0;
	scale[0][9][2][(10+Tone)%12] = 0;
	scale[0][9][2][(11+Tone)%12] = -9;

	//Sétima abaixo

	scale[0][10][0][(0+Tone)%12] = -10;
	scale[0][10][0][(1+Tone)%12] = 0;
	scale[0][10][0][(2+Tone)%12] = -10;
	scale[0][10][0][(3+Tone)%12] = 0;
	scale[0][10][0][(4+Tone)%12] = -11;
	scale[0][10][0][(5+Tone)%12] = -10;
	scale[0][10][0][(6+Tone)%12] = 0;
	scale[0][10][0][(7+Tone)%12] = -10;
	scale[0][10][0][(8+Tone)%12] = 0;
	scale[0][10][0][(9+Tone)%12] = -10;
	scale[0][10][0][(10+Tone)%12] = 0;
	scale[0][10][0][(11+Tone)%12] = -11;
	scale[0][10][1][(0+Tone)%12] = -10;
	scale[0][10][1][(1+Tone)%12] = 0;
	scale[0][10][1][(2+Tone)%12] = -11;
	scale[0][10][1][(3+Tone)%12] = -10;
	scale[0][10][1][(4+Tone)%12] = 0;
	scale[0][10][1][(5+Tone)%12] = -10;
	scale[0][10][1][(6+Tone)%12] = 0;
	scale[0][10][1][(7+Tone)%12] = -11;
	scale[0][10][1][(8+Tone)%12] = -10;
	scale[0][10][1][(9+Tone)%12] = 0;
	scale[0][10][1][(10+Tone)%12] = -10;
	scale[0][10][1][(11+Tone)%12] = 0;
	scale[0][10][2][(0+Tone)%12] = -10;
	scale[0][10][2][(1+Tone)%12] = 0;
	scale[0][10][2][(2+Tone)%12] = -11;
	scale[0][10][2][(3+Tone)%12] = -10;
	scale[0][10][2][(4+Tone)%12] = 0;
	scale[0][10][2][(5+Tone)%12] = -10;
	scale[0][10][2][(6+Tone)%12] = 0;
	scale[0][10][2][(7+Tone)%12] = -11;
	scale[0][10][2][(8+Tone)%12] = -9;
	scale[0][10][2][(9+Tone)%12] = 0;
	scale[0][10][2][(10+Tone)%12] = 0;
	scale[0][10][2][(11+Tone)%12] = -11;

	//Oitava abaixo

	scale[0][11][0][(0+Tone)%12] = -12;
	scale[0][11][0][(1+Tone)%12] = 0;
	scale[0][11][0][(2+Tone)%12] = -12;
	scale[0][11][0][(3+Tone)%12] = 0;
	scale[0][11][0][(4+Tone)%12] = -12;
	scale[0][11][0][(5+Tone)%12] = -12;
	scale[0][11][0][(6+Tone)%12] = 0;
	scale[0][11][0][(7+Tone)%12] = -12;
	scale[0][11][0][(8+Tone)%12] = 0;
	scale[0][11][0][(9+Tone)%12] = -12;
	scale[0][11][0][(10+Tone)%12] = 0;
	scale[0][11][0][(11+Tone)%12] = -12;
	scale[0][11][1][(0+Tone)%12] = -12;
	scale[0][11][1][(1+Tone)%12] = 0;
	scale[0][11][1][(2+Tone)%12] = -12;
	scale[0][11][1][(3+Tone)%12] = -12;
	scale[0][11][1][(4+Tone)%12] = 0;
	scale[0][11][1][(5+Tone)%12] = -12;
	scale[0][11][1][(6+Tone)%12] = 0;
	scale[0][11][1][(7+Tone)%12] = -12;
	scale[0][11][1][(8+Tone)%12] = -12;
	scale[0][11][1][(9+Tone)%12] = 0;
	scale[0][11][1][(10+Tone)%12] = -12;
	scale[0][11][1][(11+Tone)%12] = 0;
	scale[0][11][2][(0+Tone)%12] = -12;
	scale[0][11][2][(1+Tone)%12] = 0;
	scale[0][11][2][(2+Tone)%12] = -12;
	scale[0][11][2][(3+Tone)%12] = -12;
	scale[0][11][2][(4+Tone)%12] = 0;
	scale[0][11][2][(5+Tone)%12] = -12;
	scale[0][11][2][(6+Tone)%12] = 0;
	scale[0][11][2][(7+Tone)%12] = -12;
	scale[0][11][2][(8+Tone)%12] = -12;
	scale[0][11][2][(9+Tone)%12] = 0;
	scale[0][11][2][(10+Tone)%12] = 0;
	scale[0][11][2][(11+Tone)%12] = -12;

	
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
	
	//Quarta acima

	scale[1][1][0][(0+Tone)%12] = 5;
	scale[1][1][0][(1+Tone)%12] = 4;
	scale[1][1][0][(2+Tone)%12] = 5;
	scale[1][1][0][(3+Tone)%12] = 4;
	scale[1][1][0][(4+Tone)%12] = 5;
	scale[1][1][0][(5+Tone)%12] = 6;
	scale[1][1][0][(6+Tone)%12] = 5;
	scale[1][1][0][(7+Tone)%12] = 5;
	scale[1][1][0][(8+Tone)%12] = 4;
	scale[1][1][0][(9+Tone)%12] = 5;
	scale[1][1][0][(10+Tone)%12] = 4;
	scale[1][1][0][(11+Tone)%12] = 5;
	scale[1][1][1][(0+Tone)%12] = 5;
	scale[1][1][1][(1+Tone)%12] = 4;
	scale[1][1][1][(2+Tone)%12] = 5;
	scale[1][1][1][(3+Tone)%12] = 5;
	scale[1][1][1][(4+Tone)%12] = 4;
	scale[1][1][1][(5+Tone)%12] = 5;
	scale[1][1][1][(6+Tone)%12] = 4;
	scale[1][1][1][(7+Tone)%12] = 5;
	scale[1][1][1][(8+Tone)%12] = 6;
	scale[1][1][1][(9+Tone)%12] = 5;
	scale[1][1][1][(10+Tone)%12] = 5;
	scale[1][1][1][(11+Tone)%12] = 4;
	scale[1][1][2][(0+Tone)%12] = 5;
	scale[1][1][2][(1+Tone)%12] = 4;
	scale[1][1][2][(2+Tone)%12] = 5;
	scale[1][1][2][(3+Tone)%12] = 5;
	scale[1][1][2][(4+Tone)%12] = 4;
	scale[1][1][2][(5+Tone)%12] = 6;
	scale[1][1][2][(6+Tone)%12] = 5;
	scale[1][1][2][(7+Tone)%12] = 5;
	scale[1][1][2][(8+Tone)%12] = 6;
	scale[1][1][2][(9+Tone)%12] = 5;
	scale[1][1][2][(11+Tone)%12] = 4;
	scale[1][1][2][(10+Tone)%12] = 4;

	//Quinta acima

	scale[1][2][0][(0+Tone)%12] = 7;
	scale[1][2][0][(1+Tone)%12] = 6;
	scale[1][2][0][(2+Tone)%12] = 7;
	scale[1][2][0][(3+Tone)%12] = 6;
	scale[1][2][0][(4+Tone)%12] = 7;
	scale[1][2][0][(5+Tone)%12] = 7;
	scale[1][2][0][(6+Tone)%12] = 6;
	scale[1][2][0][(7+Tone)%12] = 7;
	scale[1][2][0][(8+Tone)%12] = 6;
	scale[1][2][0][(9+Tone)%12] = 7;
	scale[1][2][0][(10+Tone)%12] = 6;
	scale[1][2][0][(11+Tone)%12] = 6;
	scale[1][2][1][(0+Tone)%12] = 7;
	scale[1][2][1][(1+Tone)%12] = 6;
	scale[1][2][1][(2+Tone)%12] = 6;
	scale[1][2][1][(3+Tone)%12] = 7;
	scale[1][2][1][(4+Tone)%12] = 6;
	scale[1][2][1][(5+Tone)%12] = 7;
	scale[1][2][0][(0+Tone)%12] = 7;
	scale[1][2][1][(6+Tone)%12] = 6;
	scale[1][2][1][(7+Tone)%12] = 7;
	scale[1][2][1][(8+Tone)%12] = 7;
	scale[1][2][1][(9+Tone)%12] = 6;
	scale[1][2][1][(10+Tone)%12] = 7;
	scale[1][2][1][(11+Tone)%12] = 6;
	scale[1][2][2][(0+Tone)%12] = 7;
	scale[1][2][2][(1+Tone)%12] = 6;
	scale[1][2][2][(2+Tone)%12] = 6;
	scale[1][2][2][(3+Tone)%12] = 8;
	scale[1][2][2][(4+Tone)%12] = 7;
	scale[1][2][2][(5+Tone)%12] = 7;
	scale[1][2][2][(6+Tone)%12] = 6;
	scale[1][2][2][(7+Tone)%12] = 7;
	scale[1][2][2][(8+Tone)%12] = 7;
	scale[1][2][2][(9+Tone)%12] = 6;
	scale[1][2][2][(10+Tone)%12] = 5;
	scale[1][2][2][(11+Tone)%12] = 6;

	//Sexta acima

	scale[1][3][0][(0+Tone)%12] = 9;
	scale[1][3][0][(1+Tone)%12] = 8;
	scale[1][3][0][(2+Tone)%12] = 9;
	scale[1][3][0][(3+Tone)%12] = 8;
	scale[1][3][0][(4+Tone)%12] = 8;
	scale[1][3][0][(5+Tone)%12] = 9;
	scale[1][3][0][(6+Tone)%12] = 8;
	scale[1][3][0][(7+Tone)%12] = 9;
	scale[1][3][0][(8+Tone)%12] = 8;
	scale[1][3][0][(9+Tone)%12] = 8;
	scale[1][3][0][(10+Tone)%12] = 9;
	scale[1][3][0][(11+Tone)%12] = 8;
	scale[1][3][1][(0+Tone)%12] = 8;
	scale[1][3][1][(1+Tone)%12] = 9;
	scale[1][3][1][(2+Tone)%12] = 8;
	scale[1][3][1][(3+Tone)%12] = 9;
	scale[1][3][1][(4+Tone)%12] = 8;
	scale[1][3][1][(5+Tone)%12] = 9;
	scale[1][3][1][(6+Tone)%12] = 8;
	scale[1][3][1][(7+Tone)%12] = 8;
	scale[1][3][1][(8+Tone)%12] = 9;
	scale[1][3][1][(9+Tone)%12] = 8;
	scale[1][3][1][(10+Tone)%12] = 9;
	scale[1][3][1][(11+Tone)%12] = 8;
	scale[1][3][2][(0+Tone)%12] = 8;
	scale[1][3][2][(1+Tone)%12] = 10;
	scale[1][3][2][(2+Tone)%12] = 9;
	scale[1][3][2][(3+Tone)%12] = 9;
	scale[1][3][2][(4+Tone)%12] = 8;
	scale[1][3][2][(5+Tone)%12] = 9;
	scale[1][3][2][(6+Tone)%12] = 8;
	scale[1][3][2][(7+Tone)%12] = 8;
	scale[1][3][2][(8+Tone)%12] = 9;
	scale[1][3][2][(9+Tone)%12] = 8;
	scale[1][3][2][(10+Tone)%12] = 9;
	scale[1][3][2][(11+Tone)%12] = 8;
	
	//Sétima acima

	scale[1][4][0][(0+Tone)%12] = 11;
	scale[1][4][0][(1+Tone)%12] = 11;
	scale[1][4][0][(2+Tone)%12] = 10;
	scale[1][4][0][(3+Tone)%12] = 11;
	scale[1][4][0][(4+Tone)%12] = 10;
	scale[1][4][0][(5+Tone)%12] = 11;
	scale[1][4][0][(6+Tone)%12] = 11;
	scale[1][4][0][(7+Tone)%12] = 10;
	scale[1][4][0][(8+Tone)%12] = 11;
	scale[1][4][0][(9+Tone)%12] = 10;
	scale[1][4][0][(10+Tone)%12] = 11;
	scale[1][4][0][(11+Tone)%12] = 10;
	scale[1][4][1][(0+Tone)%12] = 10;
	scale[1][4][1][(1+Tone)%12] = 11;
	scale[1][4][1][(2+Tone)%12] = 10;
	scale[1][4][1][(3+Tone)%12] = 11;
	scale[1][4][1][(4+Tone)%12] = 11;
	scale[1][4][1][(5+Tone)%12] = 10;
	scale[1][4][1][(6+Tone)%12] = 11;
	scale[1][4][1][(7+Tone)%12] = 10;
	scale[1][4][1][(8+Tone)%12] = 11;
	scale[1][4][1][(9+Tone)%12] = 11;
	scale[1][4][1][(10+Tone)%12] = 10;
	scale[1][4][1][(11+Tone)%12] = 11;
	scale[1][4][2][(0+Tone)%12] = 11;
	scale[1][4][2][(1+Tone)%12] = 11;
	scale[1][4][2][(2+Tone)%12] = 10;
	scale[1][4][2][(3+Tone)%12] = 11;
	scale[1][4][2][(4+Tone)%12] = 11;
	scale[1][4][2][(5+Tone)%12] = 10;
	scale[1][4][2][(6+Tone)%12] = 11;
	scale[1][4][2][(7+Tone)%12] = 10;
	scale[1][4][2][(8+Tone)%12] = 11;
	scale[1][4][2][(9+Tone)%12] = 11;
	scale[1][4][2][(10+Tone)%12] = 10;
	scale[1][4][2][(11+Tone)%12] = 9;

	//Oitava acima

	scale[1][5][0][(0+Tone)%12] = 12;
	scale[1][5][0][(1+Tone)%12] = 12;
	scale[1][5][0][(2+Tone)%12] = 12;
	scale[1][5][0][(3+Tone)%12] = 12;
	scale[1][5][0][(4+Tone)%12] = 12;
	scale[1][5][0][(5+Tone)%12] = 12;
	scale[1][5][0][(6+Tone)%12] = 12;
	scale[1][5][0][(7+Tone)%12] = 12;
	scale[1][5][0][(8+Tone)%12] = 12;
	scale[1][5][0][(9+Tone)%12] = 12;
	scale[1][5][0][(10+Tone)%12] = 12;
	scale[1][5][0][(11+Tone)%12] = 12;
	scale[1][5][1][(0+Tone)%12] = 12;
	scale[1][5][1][(1+Tone)%12] = 12;
	scale[1][5][1][(2+Tone)%12] = 12;
	scale[1][5][1][(3+Tone)%12] = 12;
	scale[1][5][1][(4+Tone)%12] = 12;
	scale[1][5][1][(5+Tone)%12] = 12;
	scale[1][5][1][(6+Tone)%12] = 12;
	scale[1][5][1][(7+Tone)%12] = 12;
	scale[1][5][1][(8+Tone)%12] = 12;
	scale[1][5][1][(9+Tone)%12] = 12;
	scale[1][5][1][(10+Tone)%12] = 12;
	scale[1][5][1][(11+Tone)%12] = 12;
	scale[1][5][2][(0+Tone)%12] = 12;
	scale[1][5][2][(1+Tone)%12] = 12;
	scale[1][5][2][(2+Tone)%12] = 12;
	scale[1][5][2][(3+Tone)%12] = 12;
	scale[1][5][2][(4+Tone)%12] = 12;
	scale[1][5][2][(5+Tone)%12] = 12;
	scale[1][5][2][(6+Tone)%12] = 12;
	scale[1][5][2][(7+Tone)%12] = 12;
	scale[1][5][2][(8+Tone)%12] = 12;
	scale[1][5][2][(9+Tone)%12] = 12;
	scale[1][5][2][(10+Tone)%12] = 12;
	scale[1][5][2][(11+Tone)%12] = 12;

	//Terça abaixo

	scale[1][6][0][(0+Tone)%12] = -3;
	scale[1][6][0][(1+Tone)%12] = -4;
	scale[1][6][0][(2+Tone)%12] = -3;
	scale[1][6][0][(3+Tone)%12] = -4;
	scale[1][6][0][(4+Tone)%12] = -4;
	scale[1][6][0][(5+Tone)%12] = -3;
	scale[1][6][0][(6+Tone)%12] = -4;
	scale[1][6][0][(7+Tone)%12] = -3;
	scale[1][6][0][(8+Tone)%12] = -4;
	scale[1][6][0][(9+Tone)%12] = -4;
	scale[1][6][0][(10+Tone)%12] = -3;
	scale[1][6][0][(11+Tone)%12] = -4;
	scale[1][6][1][(0+Tone)%12] = -4;
	scale[1][6][1][(1+Tone)%12] = -3;
	scale[1][6][1][(2+Tone)%12] = -4;
	scale[1][6][1][(3+Tone)%12] = -3;
	scale[1][6][1][(4+Tone)%12] = -4;
	scale[1][6][1][(5+Tone)%12] = -3;
	scale[1][6][1][(6+Tone)%12] = -4;
	scale[1][6][1][(7+Tone)%12] = -4;
	scale[1][6][1][(8+Tone)%12] = -3;
	scale[1][6][1][(9+Tone)%12] = -4;
	scale[1][6][1][(10+Tone)%12] = -3;
	scale[1][6][1][(11+Tone)%12] = -4;
	scale[1][6][2][(0+Tone)%12] = -4;
	scale[1][6][2][(1+Tone)%12] = -5;
	scale[1][6][2][(2+Tone)%12] = -3;
	scale[1][6][2][(3+Tone)%12] = -3;
	scale[1][6][2][(4+Tone)%12] = -4;
	scale[1][6][2][(5+Tone)%12] = -3;
	scale[1][6][2][(6+Tone)%12] = -4;
	scale[1][6][2][(7+Tone)%12] = -4;
	scale[1][6][2][(8+Tone)%12] = -3;
	scale[1][6][2][(9+Tone)%12] = -4;
	scale[1][6][2][(10+Tone)%12] = -3;
	scale[1][6][2][(11+Tone)%12] = -4;
	
	//Quarta abaixo

	scale[1][7][0][(0+Tone)%12] = -5;
	scale[1][7][0][(1+Tone)%12] = -6;
	scale[1][7][0][(2+Tone)%12] = -5;
	scale[1][7][0][(3+Tone)%12] = -6;
	scale[1][7][0][(4+Tone)%12] = -5;
	scale[1][7][0][(5+Tone)%12] = -5;
	scale[1][7][0][(6+Tone)%12] = -6;
	scale[1][7][0][(7+Tone)%12] = -5;
	scale[1][7][0][(8+Tone)%12] = -6;
	scale[1][7][0][(9+Tone)%12] = -5;
	scale[1][7][0][(10+Tone)%12] = -6;
	scale[1][7][0][(11+Tone)%12] = -6;
	scale[1][7][1][(0+Tone)%12] = -5;
	scale[1][7][1][(1+Tone)%12] = -6;
	scale[1][7][1][(2+Tone)%12] = -6;
	scale[1][7][1][(3+Tone)%12] = -5;
	scale[1][7][1][(4+Tone)%12] = -6;
	scale[1][7][1][(5+Tone)%12] = -5;
	scale[1][7][1][(6+Tone)%12] = -6;
	scale[1][7][1][(7+Tone)%12] = -5;
	scale[1][7][1][(8+Tone)%12] = -5;
	scale[1][7][1][(9+Tone)%12] = -6;
	scale[1][7][1][(10+Tone)%12] = -5;
	scale[1][7][1][(11+Tone)%12] = -6;
	scale[1][7][2][(0+Tone)%12] = -5;
	scale[1][7][2][(1+Tone)%12] = -6;
	scale[1][7][2][(2+Tone)%12] = -6;
	scale[1][7][2][(3+Tone)%12] = -4;
	scale[1][7][2][(4+Tone)%12] = -5;
	scale[1][7][2][(5+Tone)%12] = -5;
	scale[1][7][2][(6+Tone)%12] = -6;
	scale[1][7][2][(7+Tone)%12] = -5;
	scale[1][7][2][(8+Tone)%12] = -5;
	scale[1][7][2][(9+Tone)%12] = -6;
	scale[1][7][2][(10+Tone)%12] = -7;
	scale[1][7][2][(11+Tone)%12] = -6;

	//Quinta abaixo

	scale[1][8][0][(0+Tone)%12] = -7;
	scale[1][8][0][(1+Tone)%12] = -8;
	scale[1][8][0][(2+Tone)%12] = -7;
	scale[1][8][0][(3+Tone)%12] = -8;
	scale[1][8][0][(4+Tone)%12] = -7;
	scale[1][8][0][(5+Tone)%12] = -6;
	scale[1][8][0][(6+Tone)%12] = -7;
	scale[1][8][0][(7+Tone)%12] = -7;
	scale[1][8][0][(8+Tone)%12] = -8;
	scale[1][8][0][(9+Tone)%12] = -7;
	scale[1][8][0][(10+Tone)%12] = -8;
	scale[1][8][0][(11+Tone)%12] = -7;
	scale[1][8][1][(0+Tone)%12] = -7;
	scale[1][8][1][(1+Tone)%12] = -8;
	scale[1][8][1][(2+Tone)%12] = -7;
	scale[1][8][1][(3+Tone)%12] = -7;
	scale[1][8][1][(4+Tone)%12] = -8;
	scale[1][8][1][(5+Tone)%12] = -7;
	scale[1][8][1][(6+Tone)%12] = -8;
	scale[1][8][1][(7+Tone)%12] = -7;
	scale[1][8][1][(8+Tone)%12] = -6;
	scale[1][8][1][(9+Tone)%12] = -7;
	scale[1][8][1][(10+Tone)%12] = -7;
	scale[1][8][1][(11+Tone)%12] = -8;
	scale[1][8][2][(0+Tone)%12] = -7;
	scale[1][8][2][(1+Tone)%12] = -8;
	scale[1][8][2][(2+Tone)%12] = -7;
	scale[1][8][2][(3+Tone)%12] = -7;
	scale[1][8][2][(4+Tone)%12] = -8;
	scale[1][8][2][(5+Tone)%12] = -6;
	scale[1][8][2][(6+Tone)%12] = -7;
	scale[1][8][2][(7+Tone)%12] = -7;
	scale[1][8][2][(8+Tone)%12] = -6;
	scale[1][8][2][(9+Tone)%12] = -7;
	scale[1][8][2][(11+Tone)%12] = -8;
	scale[1][8][2][(10+Tone)%12] = -8;

	//Sexta abaixo

	scale[1][9][0][(0+Tone)%12] = -8;
	scale[1][9][0][(1+Tone)%12] = -9;
	scale[1][9][0][(2+Tone)%12] = -9;
	scale[1][9][0][(3+Tone)%12] = -8;
	scale[1][9][0][(4+Tone)%12] = -9;
	scale[1][9][0][(5+Tone)%12] = -8;
	scale[1][9][0][(6+Tone)%12] = -9;
	scale[1][9][0][(7+Tone)%12] = -8;
	scale[1][9][0][(8+Tone)%12] = -9;
	scale[1][9][0][(9+Tone)%12] = -9;
	scale[1][9][0][(10+Tone)%12] = -8;
	scale[1][9][0][(11+Tone)%12] = -9;	
	scale[1][9][1][(0+Tone)%12] = -9;
	scale[1][9][1][(1+Tone)%12] = -8;
	scale[1][9][1][(2+Tone)%12] = -9;
	scale[1][9][1][(3+Tone)%12] = -8;
	scale[1][9][1][(4+Tone)%12] = -9;
	scale[1][9][1][(5+Tone)%12] = -9;
	scale[1][9][1][(6+Tone)%12] = -8;
	scale[1][9][1][(7+Tone)%12] = -9;
	scale[1][9][1][(8+Tone)%12] = -8;
	scale[1][9][1][(9+Tone)%12] = -9;
	scale[1][9][1][(10+Tone)%12] = -8;
	scale[1][9][1][(11+Tone)%12] = -9;
	scale[1][9][2][(0+Tone)%12] = -9;
	scale[1][9][2][(1+Tone)%12] = -8;
	scale[1][9][2][(2+Tone)%12] = -9;
	scale[1][9][2][(3+Tone)%12] = -8;
	scale[1][9][2][(4+Tone)%12] = -9;
	scale[1][9][2][(5+Tone)%12] = -9;
	scale[1][9][2][(6+Tone)%12] = -7;
	scale[1][9][2][(7+Tone)%12] = -8;
	scale[1][9][2][(8+Tone)%12] = -8;
	scale[1][9][2][(9+Tone)%12] = -9;
	scale[1][9][2][(10+Tone)%12] = -8;
	scale[1][9][2][(11+Tone)%12] = -9;

	//Sétima abaixo

	scale[1][10][0][(0+Tone)%12] = -10;
	scale[1][10][0][(1+Tone)%12] = -11;
	scale[1][10][0][(2+Tone)%12] = -10;
	scale[1][10][0][(3+Tone)%12] = -11;
	scale[1][10][0][(4+Tone)%12] = -11;
	scale[1][10][0][(5+Tone)%12] = -10;
	scale[1][10][0][(6+Tone)%12] = -11;
	scale[1][10][0][(7+Tone)%12] = -10;
	scale[1][10][0][(8+Tone)%12] = -11;
	scale[1][10][0][(9+Tone)%12] = -10;
	scale[1][10][0][(10+Tone)%12] = -11;
	scale[1][10][0][(11+Tone)%12] = -11;
	scale[1][10][1][(0+Tone)%12] = -10;
	scale[1][10][1][(1+Tone)%12] = -11;
	scale[1][10][1][(2+Tone)%12] = -11;
	scale[1][10][1][(3+Tone)%12] = -10;
	scale[1][10][1][(4+Tone)%12] = -11;
	scale[1][10][1][(5+Tone)%12] = -10;
	scale[1][10][1][(6+Tone)%12] = -11;
	scale[1][10][1][(7+Tone)%12] = -11;
	scale[1][10][1][(8+Tone)%12] = -10;
	scale[1][10][1][(9+Tone)%12] = -11;
	scale[1][10][1][(10+Tone)%12] = -10;
	scale[1][10][1][(11+Tone)%12] = -11;
	scale[1][10][2][(0+Tone)%12] = -10;
	scale[1][10][2][(1+Tone)%12] = -11;
	scale[1][10][2][(2+Tone)%12] = -11;
	scale[1][10][2][(3+Tone)%12] = -10;
	scale[1][10][2][(4+Tone)%12] = -11;
	scale[1][10][2][(5+Tone)%12] = -10;
	scale[1][10][2][(6+Tone)%12] = -11;
	scale[1][10][2][(7+Tone)%12] = -11;
	scale[1][10][2][(8+Tone)%12] = -9;
	scale[1][10][2][(9+Tone)%12] = -10;
	scale[1][10][2][(10+Tone)%12] = -11;
	scale[1][10][2][(11+Tone)%12] = -11;
	
	//Oitava abaixo

	scale[1][11][0][(0+Tone)%12] = -12;
	scale[1][11][0][(1+Tone)%12] = -12;
	scale[1][11][0][(2+Tone)%12] = -12;
	scale[1][11][0][(3+Tone)%12] = -12;
	scale[1][11][0][(4+Tone)%12] = -12;
	scale[1][11][0][(5+Tone)%12] = -12;
	scale[1][11][0][(6+Tone)%12] = -12;
	scale[1][11][0][(7+Tone)%12] = -12;
	scale[1][11][0][(8+Tone)%12] = -12;
	scale[1][11][0][(9+Tone)%12] = -12;
	scale[1][11][0][(10+Tone)%12] = -12;
	scale[1][11][0][(11+Tone)%12] = -12;
	scale[1][11][1][(0+Tone)%12] = -12;
	scale[1][11][1][(1+Tone)%12] = -12;
	scale[1][11][1][(2+Tone)%12] = -12;
	scale[1][11][1][(3+Tone)%12] = -12;
	scale[1][11][1][(4+Tone)%12] = -12;
	scale[1][11][1][(5+Tone)%12] = -12;
	scale[1][11][1][(6+Tone)%12] = -12;
	scale[1][11][1][(7+Tone)%12] = -12;
	scale[1][11][1][(8+Tone)%12] = -12;
	scale[1][11][1][(9+Tone)%12] = -12;
	scale[1][11][1][(10+Tone)%12] = -12;
	scale[1][11][1][(11+Tone)%12] = -12;
	scale[1][11][2][(0+Tone)%12] = -12;
	scale[1][11][2][(1+Tone)%12] = -12;
	scale[1][11][2][(2+Tone)%12] = -12;
	scale[1][11][2][(3+Tone)%12] = -12;
	scale[1][11][2][(4+Tone)%12] = -12;
	scale[1][11][2][(5+Tone)%12] = -12;
	scale[1][11][2][(6+Tone)%12] = -12;
	scale[1][11][2][(7+Tone)%12] = -12;
	scale[1][11][2][(8+Tone)%12] = -12;
	scale[1][11][2][(9+Tone)%12] = -12;
	scale[1][11][2][(10+Tone)%12] = -12;
	scale[1][11][2][(11+Tone)%12] = -12;

	
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
	
	//Quarta acima

	scale[2][1][0][(0+Tone)%12] = 5;
	scale[2][1][0][(1+Tone)%12] = 5;
	scale[2][1][0][(2+Tone)%12] = 5;
	scale[2][1][0][(3+Tone)%12] = 5;
	scale[2][1][0][(4+Tone)%12] = 5;
	scale[2][1][0][(5+Tone)%12] = 6;
	scale[2][1][0][(6+Tone)%12] = 5.5;
	scale[2][1][0][(7+Tone)%12] = 5;
	scale[2][1][0][(8+Tone)%12] = 5;
	scale[2][1][0][(9+Tone)%12] = 5;
	scale[2][1][0][(10+Tone)%12] = 5;
	scale[2][1][0][(11+Tone)%12] = 5;
	scale[2][1][1][(0+Tone)%12] = 5;
	scale[2][1][1][(1+Tone)%12] = 5;
	scale[2][1][1][(2+Tone)%12] = 5;
	scale[2][1][1][(3+Tone)%12] = 5;
	scale[2][1][1][(4+Tone)%12] = 5;
	scale[2][1][1][(5+Tone)%12] = 5;
	scale[2][1][1][(6+Tone)%12] = 5;
	scale[2][1][1][(7+Tone)%12] = 5;
	scale[2][1][1][(8+Tone)%12] = 6;
	scale[2][1][1][(9+Tone)%12] = 5.5;
	scale[2][1][1][(10+Tone)%12] = 5;
	scale[2][1][1][(11+Tone)%12] = 5;
	scale[2][1][2][(0+Tone)%12] = 5;
	scale[2][1][2][(1+Tone)%12] = 5;
	scale[2][1][2][(2+Tone)%12] = 5;
	scale[2][1][2][(3+Tone)%12] = 5;
	scale[2][1][2][(4+Tone)%12] = 5.5;
	scale[2][1][2][(5+Tone)%12] = 6;
	scale[2][1][2][(6+Tone)%12] = 5.5;
	scale[2][1][2][(7+Tone)%12] = 5;
	scale[2][1][2][(8+Tone)%12] = 6;
	scale[2][1][2][(9+Tone)%12] = 5.33;
	scale[2][1][2][(10+Tone)%12] = 4.66;
	scale[2][1][2][(11+Tone)%12] = 4;

	//Quinta acima

	scale[2][2][0][(0+Tone)%12] = 7;
	scale[2][2][0][(1+Tone)%12] = 7;
	scale[2][2][0][(2+Tone)%12] = 7;
	scale[2][2][0][(3+Tone)%12] = 7;
	scale[2][2][0][(4+Tone)%12] = 7;
	scale[2][2][0][(5+Tone)%12] = 7;
	scale[2][2][0][(6+Tone)%12] = 7;
	scale[2][2][0][(7+Tone)%12] = 7;
	scale[2][2][0][(8+Tone)%12] = 7;
	scale[2][2][0][(9+Tone)%12] = 7;
	scale[2][2][0][(10+Tone)%12] = 6.5;
	scale[2][2][0][(11+Tone)%12] = 6;
	scale[2][2][1][(0+Tone)%12] = 7;
	scale[2][2][1][(1+Tone)%12] = 6.5;
	scale[2][2][1][(2+Tone)%12] = 6;
	scale[2][2][1][(3+Tone)%12] = 7;
	scale[2][2][1][(4+Tone)%12] = 7;
	scale[2][2][1][(5+Tone)%12] = 7;
	scale[2][2][1][(6+Tone)%12] = 7;
	scale[2][2][1][(7+Tone)%12] = 7;
	scale[2][2][1][(8+Tone)%12] = 7;
	scale[2][2][1][(9+Tone)%12] = 7;
	scale[2][2][1][(10+Tone)%12] = 7;
	scale[2][2][1][(11+Tone)%12] = 7;
	scale[2][2][2][(0+Tone)%12] = 7;
	scale[2][2][2][(1+Tone)%12] = 6.5;
	scale[2][2][2][(2+Tone)%12] = 6;
	scale[2][2][2][(3+Tone)%12] = 8;
	scale[2][2][2][(4+Tone)%12] = 7.5;
	scale[2][2][2][(5+Tone)%12] = 7;
	scale[2][2][2][(6+Tone)%12] = 7;
	scale[2][2][2][(7+Tone)%12] = 7;
	scale[2][2][2][(8+Tone)%12] = 7;
	scale[2][2][2][(9+Tone)%12] = 6.66;
	scale[2][2][2][(10+Tone)%12] = 6.33;
	scale[2][2][2][(11+Tone)%12] = 6;

	//Sexta acima

	scale[2][3][0][(0+Tone)%12] = 9;
	scale[2][3][0][(1+Tone)%12] = 9;
	scale[2][3][0][(2+Tone)%12] = 9;
	scale[2][3][0][(3+Tone)%12] = 8.5;
	scale[2][3][0][(4+Tone)%12] = 8;
	scale[2][3][0][(5+Tone)%12] = 9;
	scale[2][3][0][(6+Tone)%12] = 9;
	scale[2][3][0][(7+Tone)%12] = 9;
	scale[2][3][0][(8+Tone)%12] = 8.5;
	scale[2][3][0][(9+Tone)%12] = 8;
	scale[2][3][0][(10+Tone)%12] = 8;
	scale[2][3][0][(11+Tone)%12] = 8;
	scale[2][3][1][(0+Tone)%12] = 8;
	scale[2][3][1][(1+Tone)%12] = 8;
	scale[2][3][1][(2+Tone)%12] = 8;
	scale[2][3][1][(3+Tone)%12] = 9;
	scale[2][3][1][(4+Tone)%12] = 9;
	scale[2][3][1][(5+Tone)%12] = 9;
	scale[2][3][1][(6+Tone)%12] = 8.5;
	scale[2][3][1][(7+Tone)%12] = 8;
	scale[2][3][1][(8+Tone)%12] = 9;
	scale[2][3][1][(9+Tone)%12] = 9;
	scale[2][3][1][(10+Tone)%12] = 9;
	scale[2][3][1][(11+Tone)%12] = 8.5;
	scale[2][3][2][(0+Tone)%12] = 8;
	scale[2][3][2][(1+Tone)%12] = 8.5;
	scale[2][3][2][(2+Tone)%12] = 9;
	scale[2][3][2][(3+Tone)%12] = 9;
	scale[2][3][2][(4+Tone)%12] = 9;
	scale[2][3][2][(5+Tone)%12] = 9;
	scale[2][3][2][(6+Tone)%12] = 8.5;
	scale[2][3][2][(7+Tone)%12] = 8;
	scale[2][3][2][(8+Tone)%12] = 9;
	scale[2][3][2][(9+Tone)%12] = 8.66;
	scale[2][3][2][(10+Tone)%12] = 8.33;
	scale[2][3][2][(11+Tone)%12] = 8;
	
	//Sétima acima

	scale[2][4][0][(0+Tone)%12] = 11;
	scale[2][4][0][(1+Tone)%12] = 10.5;
	scale[2][4][0][(2+Tone)%12] = 10;
	scale[2][4][0][(3+Tone)%12] = 10;
	scale[2][4][0][(4+Tone)%12] = 10;
	scale[2][4][0][(5+Tone)%12] = 11;
	scale[2][4][0][(6+Tone)%12] = 10.5;
	scale[2][4][0][(7+Tone)%12] = 10;
	scale[2][4][0][(8+Tone)%12] = 10;
	scale[2][4][0][(9+Tone)%12] = 10;
	scale[2][4][0][(10+Tone)%12] = 10;
	scale[2][4][0][(11+Tone)%12] = 10;
	scale[2][4][1][(0+Tone)%12] = 10;
	scale[2][4][1][(1+Tone)%12] = 10;
	scale[2][4][1][(2+Tone)%12] = 10;
	scale[2][4][1][(3+Tone)%12] = 11;
	scale[2][4][1][(4+Tone)%12] = 10.5;
	scale[2][4][1][(5+Tone)%12] = 10;
	scale[2][4][1][(6+Tone)%12] = 10;
	scale[2][4][1][(7+Tone)%12] = 10;
	scale[2][4][1][(8+Tone)%12] = 11;
	scale[2][4][1][(9+Tone)%12] = 10.5;
	scale[2][4][1][(10+Tone)%12] = 10;
	scale[2][4][1][(11+Tone)%12] = 10;
	scale[2][4][2][(0+Tone)%12] = 11;
	scale[2][4][2][(1+Tone)%12] = 10.5;
	scale[2][4][2][(2+Tone)%12] = 10;
	scale[2][4][2][(3+Tone)%12] = 11;
	scale[2][4][2][(4+Tone)%12] = 10.5;
	scale[2][4][2][(5+Tone)%12] = 10;
	scale[2][4][2][(6+Tone)%12] = 10;
	scale[2][4][2][(7+Tone)%12] = 10;
	scale[2][4][2][(8+Tone)%12] = 11;
	scale[2][4][2][(9+Tone)%12] = 10.33;
	scale[2][4][2][(10+Tone)%12] = 9.66;
	scale[2][4][2][(11+Tone)%12] = 9;

	//Oitava acima

	scale[2][5][0][(0+Tone)%12] = 12;
	scale[2][5][0][(1+Tone)%12] = 12;
	scale[2][5][0][(2+Tone)%12] = 12;
	scale[2][5][0][(3+Tone)%12] = 12;
	scale[2][5][0][(4+Tone)%12] = 12;
	scale[2][5][0][(5+Tone)%12] = 12;
	scale[2][5][0][(6+Tone)%12] = 12;
	scale[2][5][0][(7+Tone)%12] = 12;
	scale[2][5][0][(8+Tone)%12] = 12;
	scale[2][5][0][(9+Tone)%12] = 12;
	scale[2][5][0][(10+Tone)%12] = 12;
	scale[2][5][0][(11+Tone)%12] = 12;
	scale[2][5][1][(0+Tone)%12] = 12;
	scale[2][5][1][(1+Tone)%12] = 12;
	scale[2][5][1][(2+Tone)%12] = 12;
	scale[2][5][1][(3+Tone)%12] = 12;
	scale[2][5][1][(4+Tone)%12] = 12;
	scale[2][5][1][(5+Tone)%12] = 12;
	scale[2][5][1][(6+Tone)%12] = 12;
	scale[2][5][1][(7+Tone)%12] = 12;
	scale[2][5][1][(8+Tone)%12] = 12;
	scale[2][5][1][(9+Tone)%12] = 12;
	scale[2][5][1][(10+Tone)%12] = 12;
	scale[2][5][1][(11+Tone)%12] = 12;
	scale[2][5][2][(0+Tone)%12] = 12;
	scale[2][5][2][(1+Tone)%12] = 12;
	scale[2][5][2][(2+Tone)%12] = 12;
	scale[2][5][2][(3+Tone)%12] = 12;
	scale[2][5][2][(4+Tone)%12] = 12;
	scale[2][5][2][(5+Tone)%12] = 12;
	scale[2][5][2][(6+Tone)%12] = 12;
	scale[2][5][2][(7+Tone)%12] = 12;
	scale[2][5][2][(8+Tone)%12] = 12;
	scale[2][5][2][(9+Tone)%12] = 12;
	scale[2][5][2][(10+Tone)%12] = 12;
	scale[2][5][2][(11+Tone)%12] = 12;

	//Terça abaixo

	scale[2][6][0][(0+Tone)%12] = -3;
	scale[2][6][0][(1+Tone)%12] = -3;
	scale[2][6][0][(2+Tone)%12] = -3;
	scale[2][6][0][(3+Tone)%12] = -3.5;
	scale[2][6][0][(4+Tone)%12] = -4;
	scale[2][6][0][(5+Tone)%12] = -3;
	scale[2][6][0][(6+Tone)%12] = -3;
	scale[2][6][0][(7+Tone)%12] = -3;
	scale[2][6][0][(8+Tone)%12] = -3.5;
	scale[2][6][0][(9+Tone)%12] = -4;
	scale[2][6][0][(10+Tone)%12] = -4;
	scale[2][6][0][(11+Tone)%12] = -4;
	scale[2][6][1][(0+Tone)%12] = -4;
	scale[2][6][1][(1+Tone)%12] = -4;
	scale[2][6][1][(2+Tone)%12] = -4;
	scale[2][6][1][(3+Tone)%12] = -3;
	scale[2][6][1][(4+Tone)%12] = -3;
	scale[2][6][1][(5+Tone)%12] = -3;
	scale[2][6][1][(6+Tone)%12] = -3.5;
	scale[2][6][1][(7+Tone)%12] = -4;
	scale[2][6][1][(8+Tone)%12] = -3;
	scale[2][6][1][(9+Tone)%12] = -3;
	scale[2][6][1][(10+Tone)%12] = -3;
	scale[2][6][1][(11+Tone)%12] = -3.5;
	scale[2][6][2][(0+Tone)%12] = -4;
	scale[2][6][2][(1+Tone)%12] = -3.5;
	scale[2][6][2][(2+Tone)%12] = -3;
	scale[2][6][2][(3+Tone)%12] = -3;
	scale[2][6][2][(4+Tone)%12] = -3;
	scale[2][6][2][(5+Tone)%12] = -3;
	scale[2][6][2][(6+Tone)%12] = -3.5;
	scale[2][6][2][(7+Tone)%12] = -4;
	scale[2][6][2][(8+Tone)%12] = -3;
	scale[2][6][2][(9+Tone)%12] = -3.33;
	scale[2][6][2][(10+Tone)%12] = -3.66;
	scale[2][6][2][(11+Tone)%12] = -4;
	
	//Quarta abaixo

	scale[2][7][0][(0+Tone)%12] = -5;
	scale[2][7][0][(1+Tone)%12] = -5;
	scale[2][7][0][(2+Tone)%12] = -5;
	scale[2][7][0][(3+Tone)%12] = -5;
	scale[2][7][0][(4+Tone)%12] = -5;
	scale[2][7][0][(5+Tone)%12] = -5;
	scale[2][7][0][(6+Tone)%12] = -5;
	scale[2][7][0][(7+Tone)%12] = -5;
	scale[2][7][0][(8+Tone)%12] = -5;
	scale[2][7][0][(9+Tone)%12] = -5;
	scale[2][7][0][(10+Tone)%12] = -5.5;
	scale[2][7][0][(11+Tone)%12] = -6;
	scale[2][7][1][(0+Tone)%12] = -5;
	scale[2][7][1][(1+Tone)%12] = -5.5;
	scale[2][7][1][(2+Tone)%12] = -6;
	scale[2][7][1][(3+Tone)%12] = -5;
	scale[2][7][1][(4+Tone)%12] = -5;
	scale[2][7][1][(5+Tone)%12] = -5;
	scale[2][7][1][(6+Tone)%12] = -5;
	scale[2][7][1][(7+Tone)%12] = -5;
	scale[2][7][1][(8+Tone)%12] = -5;
	scale[2][7][1][(9+Tone)%12] = -5;
	scale[2][7][1][(10+Tone)%12] = -5;
	scale[2][7][1][(11+Tone)%12] = -5;
	scale[2][7][2][(0+Tone)%12] = -5;
	scale[2][7][2][(1+Tone)%12] = -5.5;
	scale[2][7][2][(2+Tone)%12] = -6;
	scale[2][7][2][(3+Tone)%12] = -4;
	scale[2][7][2][(4+Tone)%12] = -4.5;
	scale[2][7][2][(5+Tone)%12] = -5;
	scale[2][7][2][(6+Tone)%12] = -5;
	scale[2][7][2][(7+Tone)%12] = -5;
	scale[2][7][2][(8+Tone)%12] = -5;
	scale[2][7][2][(9+Tone)%12] = -5.33;
	scale[2][7][2][(10+Tone)%12] = -5.66;
	scale[2][7][2][(11+Tone)%12] = -6;

	//Quinta abaixo

	scale[2][8][0][(0+Tone)%12] = -7;
	scale[2][8][0][(1+Tone)%12] = -7;
	scale[2][8][0][(2+Tone)%12] = -7;
	scale[2][8][0][(3+Tone)%12] = -7;
	scale[2][8][0][(4+Tone)%12] = -7;
	scale[2][8][0][(5+Tone)%12] = -6;
	scale[2][8][0][(6+Tone)%12] = -6.5;
	scale[2][8][0][(7+Tone)%12] = -7;
	scale[2][8][0][(8+Tone)%12] = -7;
	scale[2][8][0][(9+Tone)%12] = -7;
	scale[2][8][0][(10+Tone)%12] = -7;
	scale[2][8][0][(11+Tone)%12] = -7;
	scale[2][8][1][(0+Tone)%12] = -7;
	scale[2][8][1][(1+Tone)%12] = -7;
	scale[2][8][1][(2+Tone)%12] = -7;
	scale[2][8][1][(3+Tone)%12] = -7;
	scale[2][8][1][(4+Tone)%12] = -7;
	scale[2][8][1][(5+Tone)%12] = -7;
	scale[2][8][1][(6+Tone)%12] = -7;
	scale[2][8][1][(7+Tone)%12] = -7;
	scale[2][8][1][(8+Tone)%12] = -6;
	scale[2][8][1][(9+Tone)%12] = -6.5;
	scale[2][8][1][(10+Tone)%12] = -7;
	scale[2][8][1][(11+Tone)%12] = -7;
	scale[2][8][2][(0+Tone)%12] = -7;
	scale[2][8][2][(1+Tone)%12] = -7;
	scale[2][8][2][(2+Tone)%12] = -7;
	scale[2][8][2][(3+Tone)%12] = -7;
	scale[2][8][2][(4+Tone)%12] = -6.5;
	scale[2][8][2][(5+Tone)%12] = -6;
	scale[2][8][2][(6+Tone)%12] = -6.5;
	scale[2][8][2][(7+Tone)%12] = -7;
	scale[2][8][2][(8+Tone)%12] = -6;
	scale[2][8][2][(9+Tone)%12] = -6.66;
	scale[2][8][2][(10+Tone)%12] = -7.33;
	scale[2][8][2][(11+Tone)%12] = -8;

	//Sexta abaixo

	scale[2][9][0][(0+Tone)%12] = -8;
	scale[2][9][0][(1+Tone)%12] = -8.5;
	scale[2][9][0][(2+Tone)%12] = -9;
	scale[2][9][0][(3+Tone)%12] = -9;
	scale[2][9][0][(4+Tone)%12] = -9;
	scale[2][9][0][(5+Tone)%12] = -8;
	scale[2][9][0][(6+Tone)%12] = -8;
	scale[2][9][0][(7+Tone)%12] = -8;
	scale[2][9][0][(8+Tone)%12] = -8.5;
	scale[2][9][0][(9+Tone)%12] = -9;
	scale[2][9][0][(10+Tone)%12] = -9;
	scale[2][9][0][(11+Tone)%12] = -9;	
	scale[2][9][1][(0+Tone)%12] = -9;
	scale[2][9][1][(1+Tone)%12] = -9;
	scale[2][9][1][(2+Tone)%12] = -9;
	scale[2][9][1][(3+Tone)%12] = -8;
	scale[2][9][1][(4+Tone)%12] = -8.5;
	scale[2][9][1][(5+Tone)%12] = -9;
	scale[2][9][1][(6+Tone)%12] = -9;
	scale[2][9][1][(7+Tone)%12] = -9;
	scale[2][9][1][(8+Tone)%12] = -8;
	scale[2][9][1][(9+Tone)%12] = -8;
	scale[2][9][1][(10+Tone)%12] = -8;
	scale[2][9][1][(11+Tone)%12] = -8.5;
	scale[2][9][2][(0+Tone)%12] = -9;
	scale[2][9][2][(1+Tone)%12] = -9;
	scale[2][9][2][(2+Tone)%12] = -9;
	scale[2][9][2][(3+Tone)%12] = -8;
	scale[2][9][2][(4+Tone)%12] = -8.5;
	scale[2][9][2][(5+Tone)%12] = -9;
	scale[2][9][2][(6+Tone)%12] = -9;
	scale[2][9][2][(7+Tone)%12] = -9;
	scale[2][9][2][(8+Tone)%12] = -8;
	scale[2][9][2][(9+Tone)%12] = -8.33;
	scale[2][9][2][(10+Tone)%12] = -8.66;
	scale[2][9][2][(11+Tone)%12] = -9;

	//Sétima abaixo

	scale[2][10][0][(0+Tone)%12] = -10;
	scale[2][10][0][(1+Tone)%12] = -10;
	scale[2][10][0][(2+Tone)%12] = -10;
	scale[2][10][0][(3+Tone)%12] = -10.5;
	scale[2][10][0][(4+Tone)%12] = -11;
	scale[2][10][0][(5+Tone)%12] = -10;
	scale[2][10][0][(6+Tone)%12] = -10;
	scale[2][10][0][(7+Tone)%12] = -10;
	scale[2][10][0][(8+Tone)%12] = -10;
	scale[2][10][0][(9+Tone)%12] = -10;
	scale[2][10][0][(10+Tone)%12] = -10.5;
	scale[2][10][0][(11+Tone)%12] = -11;
	scale[2][10][1][(0+Tone)%12] = -10;
	scale[2][10][1][(1+Tone)%12] = -10.5;
	scale[2][10][1][(2+Tone)%12] = -11;
	scale[2][10][1][(3+Tone)%12] = -10;
	scale[2][10][1][(4+Tone)%12] = -10;
	scale[2][10][1][(5+Tone)%12] = -10;
	scale[2][10][1][(6+Tone)%12] = -10.5;
	scale[2][10][1][(7+Tone)%12] = -11;
	scale[2][10][1][(8+Tone)%12] = -10;
	scale[2][10][1][(9+Tone)%12] = -10;
	scale[2][10][1][(10+Tone)%12] = -10;
	scale[2][10][1][(11+Tone)%12] = -10;
	scale[2][10][2][(0+Tone)%12] = -10;
	scale[2][10][2][(1+Tone)%12] = -10.5;
	scale[2][10][2][(2+Tone)%12] = -11;
	scale[2][10][2][(3+Tone)%12] = -10;
	scale[2][10][2][(4+Tone)%12] = -10;
	scale[2][10][2][(5+Tone)%12] = -10;
	scale[2][10][2][(6+Tone)%12] = -10.5;
	scale[2][10][2][(7+Tone)%12] = -11;
	scale[2][10][2][(8+Tone)%12] = -9;
	scale[2][10][2][(9+Tone)%12] = -9.66;
	scale[2][10][2][(10+Tone)%12] = -10.33;
	scale[2][10][2][(11+Tone)%12] = -11;

	//Oitava abaixo

	scale[2][11][0][(0+Tone)%12] = -12;
	scale[2][11][0][(1+Tone)%12] = -12;
	scale[2][11][0][(2+Tone)%12] = -12;
	scale[2][11][0][(3+Tone)%12] = -12;
	scale[2][11][0][(4+Tone)%12] = -12;
	scale[2][11][0][(5+Tone)%12] = -12;
	scale[2][11][0][(6+Tone)%12] = -12;
	scale[2][11][0][(7+Tone)%12] = -12;
	scale[2][11][0][(8+Tone)%12] = -12;
	scale[2][11][0][(9+Tone)%12] = -12;
	scale[2][11][0][(10+Tone)%12] = -12;
	scale[2][11][0][(11+Tone)%12] = -12;
	scale[2][11][1][(0+Tone)%12] = -12;
	scale[2][11][1][(1+Tone)%12] = -12;
	scale[2][11][1][(2+Tone)%12] = -12;
	scale[2][11][1][(3+Tone)%12] = -12;
	scale[2][11][1][(4+Tone)%12] = -12;
	scale[2][11][1][(5+Tone)%12] = -12;
	scale[2][11][1][(6+Tone)%12] = -12;
	scale[2][11][1][(7+Tone)%12] = -12;
	scale[2][11][1][(8+Tone)%12] = -12;
	scale[2][11][1][(9+Tone)%12] = -12;
	scale[2][11][1][(10+Tone)%12] = -12;
	scale[2][11][1][(11+Tone)%12] = -12;
	scale[2][11][2][(0+Tone)%12] = -12;
	scale[2][11][2][(1+Tone)%12] = -12;
	scale[2][11][2][(2+Tone)%12] = -12;
	scale[2][11][2][(3+Tone)%12] = -12;
	scale[2][11][2][(4+Tone)%12] = -12;
	scale[2][11][2][(5+Tone)%12] = -12;
	scale[2][11][2][(6+Tone)%12] = -12;
	scale[2][11][2][(7+Tone)%12] = -12;
	scale[2][11][2][(8+Tone)%12] = -12;
	scale[2][11][2][(9+Tone)%12] = -12;
	scale[2][11][2][(10+Tone)%12] = -12;
	scale[2][11][2][(11+Tone)%12] = -12;

	
	if (Mode < 0) Mode = 0;
	if (Mode > 2) Mode = 2;
	if (Scale < 0) Scale = 0;
	if (Scale > 2) Scale = 2;
	if (Interval < 0) Interval = 0;
	if (Interval > 11) Interval = 11;
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

void FindStepCS(int note, int oitava, int Tone, int s_0, int s_1, int s_2, int s_3, int s_4, int s_5, int s_6, int s_7, int s_8, int s_9, int s_10, int s_11,  int LowNote, int hopa, int Qcolumn, double *s, int *hops)
{
	
		float scale[12];
		

	scale[(0+Tone)%12] = s_0;
	scale[(1+Tone)%12] = s_1;
	scale[(2+Tone)%12] = s_2;
	scale[(3+Tone)%12] = s_3;
	scale[(4+Tone)%12] = s_4;
	scale[(5+Tone)%12] = s_5;
	scale[(6+Tone)%12] = s_6;
	scale[(7+Tone)%12] = s_7;
	scale[(8+Tone)%12] = s_8;
	scale[(9+Tone)%12] = s_9;
	scale[(10+Tone)%12] = s_10;
	scale[(11+Tone)%12] = s_11;
	
	if (LowNote < 0) LowNote = 0;
	if (LowNote > 14) LowNote = 14;
	
	switch (note)
	{
		case 0:
			s[0] = scale[0];
			break;
		case 1:
			s[0] = scale[1];
			break;
		case 2:
			s[0] = scale[2];
			break;
		case 3:
			s[0] = scale[3];
			break;
		case 4:
			s[0] = scale[4];
			break;
		case 5:
			s[0] = scale[5];
			break;
		case 6:
			s[0] = scale[6];
			break;
		case 7:
			s[0] = scale[7];
			break;
		case 8:
			s[0] = scale[8];
			break;
		case 9:
			s[0] = scale[9];
			break;
		case 10:
			s[0] = scale[10];
			break;
		case 11:
			s[0] = scale[11];
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
