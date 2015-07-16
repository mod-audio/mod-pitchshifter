#include "PitchDetectionClass.h"

PitchDetection::PitchDetection(uint32_t n_samples, int nBuffers, double SampleRate) //Constructor
{
	hopa = n_samples;
	N = nBuffers*n_samples;
	fs = SampleRate;

	frames = fftwf_alloc_real(2*N); memset(frames, 0, 2*N );
	b = new float*[hopa];

	for (int i=0 ; i< nBuffers; i++)
	{
		b[i] = &frames[i*hopa];
	}

	q = fftwf_alloc_real(2*N);	
	fXa = fftwf_alloc_complex(N + 1);
	fXs = fftwf_alloc_complex(N + 1);

	Xa.zeros(N + 1);
	Xs.zeros(N + 1);
	R.zeros(N);
	NORM.zeros(N);
	F.zeros(N);
	AUTO.zeros(N);

	fftwf_import_wisdom_from_filename("/etc/fftw/wisdom/my.wisdom");
	p  = fftwf_plan_dft_r2c_1d(2*N, frames, fXa, FFTW_WISDOM_ONLY);
	p2 = fftwf_plan_dft_c2r_1d(2*N, fXs, q, FFTW_WISDOM_ONLY);
}

PitchDetection::~PitchDetection() //Destructor
{
	delete[] b;

	fftwf_free(frames);
	fftwf_free(q);
	fftwf_free(fXa);
	fftwf_free(fXs);
	
	Xa.clear();
	Xs.clear();
	R.clear();
	NORM.clear();
	F.clear();
	AUTO.clear();
	
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
		Xa(i) = cx_float(fXa[i][0], fXa[i][1]);
	}
	
	Xs = Xa % conj(Xa);
	
	for (int i=0; i<(N + 1); i++)
	{
		fXs[i][0] = real(Xs(i));
		fXs[i][1] = imag(Xs(i));
	}
	
	fftwf_execute(p2);
	
	for (int i=0; i<N; i++)
	{
		R(i) = q[i]/(2*N); 
	}
	
	NORM.zeros();
	
	NORM(0) = 2*R(0);
	
	for (int i=1; i<N; i++)
	{
		NORM(i) = NORM(i-1) - pow(frames[i-1],2)- pow(frames[N-i],2);
	}
	
	
	for (int i=0; i<N; i++)
	{
		F(i) = 1 -0.05*i/(N-1);
	}
	
	AUTO = ( 2*F % R )/NORM;
	
	int flag = 0;
	
	for (int i=0; (i<N)&&(flag==0); i++)
	{
		if( AUTO(i) > 0 )
		{
			AUTO(i) = 0;
		}
		else
		{
			flag = 1;
		}
	}
	
	uword max_index;
	double fidelity = AUTO.max(max_index);
	
	
	if ((fidelity > 0.95) && (fidelity < 1) && ((int)max_index < N-1))
	{
	
		double a = AUTO(max_index-1);
		double b = AUTO(max_index);
		double c = AUTO(max_index+1);
	
		double real_index = max_index + 0.5*(a-c)/(a-2*b+c);
	
		f = fs/real_index;
		int nota = (int)round( (12/log(2))*log(f/16.351597831287414) );
		oitava = floor(nota/12.0);
		note = nota % 12;
	
		//cout << "nota = " << note[0] << " oitava = " << oitava[0] << " fidelity = " << fidelity << "\n";	
		
	}
}

