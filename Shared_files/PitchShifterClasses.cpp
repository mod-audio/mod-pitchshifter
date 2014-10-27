#include "PitchShifterClasses.h"

PSAnalysis::PSAnalysis(uint32_t n_samples, int nBuffers) //Construtor
{
	cout << "Entrei no contrutor do PSAnalysis" << endl;
	Qcolumn = nBuffers;
	hopa = n_samples;
	N = nBuffers*n_samples;

	frames = (double*)calloc(N,sizeof(double));
	b = (double**)calloc(hopa,sizeof(double*));


	for (int i=0 ; i< nBuffers; i++)
    {
		b[i] = &frames[i*hopa];
	}


	frames2 = fftwf_alloc_real(N); for (int i = 0; i < N; i++) frames2[i] = 0;
	fXa = fftwf_alloc_complex(N/2 + 1);
	Xa.zeros(N/2 + 1);
	XaPrevious.zeros(N/2 + 1);
	Xa_arg.zeros(N/2 + 1);
	XaPrevious_arg.zeros(N/2 + 1);
	d_phi.zeros(N/2 + 1); 
	d_phi_prime.zeros(N/2 + 1); 
	d_phi_wrapped.zeros(N/2 + 1); 
	omega_true_sobre_fs.zeros(N/2 + 1); 
	AUX.zeros(N/2 + 1); 
	Xa_abs.zeros(N/2 + 1); 
	w.zeros(N); hann(N,&w); 
	I.zeros(N/2 + 1); I = linspace(0, N/2, N/2 + 1); 
	p = fftwf_plan_dft_r2c_1d(N, frames2, fXa, FFTW_ESTIMATE);

	cout << "Sai do contrutor do PSAnalysis" << endl;

}

PSAnalysis::~PSAnalysis() //Destrutor
{
	cout << "Entrei no destrutor do PSAnalysis" << endl;
	fftwf_destroy_plan(p); cout << "1" << endl;
	free(b); cout << "2" << endl;
	free(frames); cout << "3" << endl;
	fftwf_free(frames2); cout << "4" << endl;
	fftwf_free(fXa); cout << "5" << endl;
	Xa.clear(); cout << "6" << endl;
	XaPrevious.clear(); cout << "7" << endl;
	Xa_arg.clear(); cout << "8" << endl;
	XaPrevious_arg.clear(); cout << "9" << endl;
    d_phi.clear(); cout << "10" << endl;
	d_phi_prime.clear(); cout << "11" << endl;
	d_phi_wrapped.clear(); cout << "12" << endl;
	omega_true_sobre_fs.clear(); cout << "13" << endl;
	AUX.clear(); cout << "14" << endl;
	Xa_abs.clear(); cout << "15" << endl;
	w.clear(); cout << "16" << endl;
	I.clear();

	cout << "Sai do destrutor do PSAnalysis" << endl;
}

void PSAnalysis::PreAnalysis(int nBuffers, float *in)
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

void PSAnalysis::Analysis()
{
	//Starts now
	
	//Windowing
	
	for (int i=1; i<=N; i++)
	{
		frames2[i-1] = frames[i-1]*w(i-1)/(sqrt(((double)N/(2*(double)hopa))));
	}
	
	/*Analysis*/
	fftwf_execute(p);
	
	/*Processing*/
	
	for (int i=1; i<=(N/2 + 1); i++)
	{
		Xa(i-1) = cx_float(fXa[i-1][0], fXa[i-1][1]);
		Xa_arg(i-1) = angle(Xa(i-1));
	}
	d_phi = Xa_arg - XaPrevious_arg;
	d_phi_prime = d_phi - ((2*M_PI * hopa) / N) * I;
	AUX = floor((d_phi_prime + M_PI) / (2*M_PI));
	d_phi_wrapped = d_phi_prime - AUX * (2*M_PI);
	omega_true_sobre_fs = (2*M_PI/N) * I + d_phi_wrapped / hopa;

	Xa_abs = abs(Xa);
	XaPrevious = Xa;
	XaPrevious_arg = Xa_arg;
}

PSSinthesis::PSSinthesis(PSAnalysis *obj) //Construtor
{
	cout << "Entrei no construtor do PSSinthesis" << endl;
	Qcolumn = obj->Qcolumn;
	hopa = obj->hopa;
	N = obj->N;
	omega_true_sobre_fs = &obj->omega_true_sobre_fs;
	Xa_abs = &obj->Xa_abs;
	w = &obj->w;

	cout << "?" << endl;

	hops = new int[Qcolumn]; for (int i = 0; i < Qcolumn;i++) hops[i] = hopa; cout << "1" << endl;
	ysaida = new double[2*N + 4*(Qcolumn-1)*hopa]; for (int i = 0; i < 2*N + 4*(Qcolumn-1)*hopa ;i++) ysaida[i] = 0; cout << "2" << endl;
	yshift = new double[hopa]; for (int i = 0; i < hopa ;i++) yshift[i] = 0; cout << "3" << endl;
	q = fftwf_alloc_real(N); cout << "4" << endl;
	fXs = fftwf_alloc_complex(N/2 + 1); cout << "5" << endl;
	Xs.zeros(N/2 + 1); cout << "6" << endl;
	Phi.zeros(N/2 + 1); cout << "7" << endl;
	PhiPrevious.zeros(N/2 + 1); cout << "8" << endl;
	p2 = fftwf_plan_dft_c2r_1d(N, fXs, q, FFTW_ESTIMATE); cout << "9" << endl;

	cout << "Sai do construtor do PSSinthesis" << endl;
}

PSSinthesis::~PSSinthesis() //Destrutor
{
	delete[] hops;
	delete[] ysaida;
	delete[] yshift;
	fftwf_free(q);
	fftwf_free(fXs);
	Xs.clear();
	Phi.clear();
	PhiPrevious.clear();
	fftwf_destroy_plan(p2);
}

void PSSinthesis::PreSinthesis()
{
	for (int k=0; k< Qcolumn-1; k++) hops[k] = hops[k+1];
}

void PSSinthesis::Sinthesis(double s)
{
	hops[Qcolumn-1] = round(hopa*(pow(2,(s/12))));	
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

	Phi = PhiPrevious + (hops[Qcolumn-1])*omega_true_sobre_fs[0] ;
	for (int i=1; i<=(N/2 + 1); i++)
	{
        Xs(i-1) = ExponencialComplexa(Phi(i-1));
	}
	Xs = Xa_abs[0] % Xs;
	PhiPrevious = Phi;
	
	
	for (int i=1; i<=(N/2 + 1); i++)
	{
        fXs[i-1][0] = real(Xs(i-1));
        fXs[i-1][1] = imag(Xs(i-1));
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