#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <complex>
#include "fftw3-ne10.h"
#include <armadillo>
#include "Exp.h"
#include "angle.h"
#include "window.h"

using namespace arma;
using namespace std;

class PSAnalysis
{
public:
    PSAnalysis(uint32_t n_samples, int nBuffers, const char* wisdomFile);
    ~PSAnalysis();
    void PreAnalysis(int nBuffers, float *in);
    void Analysis();

    int N; //Size of the frame
    int hopa; //Analysis hop
    int Qcolumn; //Number of frames that may be used in the overlap-add

    double **b;
    
    double *frames; //A frame of last N samples
    vec w; //A hanning window vector
    float *frames2; //It's the frames vector windowed
    fftwf_plan p; //FFTW plan for the FFT of frames2
    fftwf_complex *fXa; // FFT of frames2
    cx_vec Xa; // Armadillo vector of fXa
    vec Xa_arg; //Phase of Xa
    vec Xa_abs; //Modulus of Xa
    cx_vec XaPrevious;
    vec XaPrevious_arg;
    vec d_phi; //Xa_arg - XaPrevious
	vec d_phi_prime; //?
	vec I; // A vector of the N/2 integer numbers, starting from 0.
	vec AUX; //?
	vec d_phi_wrapped; //?
	vec omega_true_sobre_fs; //?
};

class PSSinthesis
{
public:
    PSSinthesis(PSAnalysis *obj, const char* wisdomFile);
    ~PSSinthesis();
    void PreSinthesis();
    void Sinthesis(double s);

    int N; //Size of the frame
    int hopa; //Analysis hop
    int Qcolumn; //Number of frames that may be used in the overlap-add
    vec *omega_true_sobre_fs; //?
    vec *Xa_abs; //Modulus of Xa
    vec *w; //A hanning window vector

    bool first;
    int *hops; //The last Qcolumn's hop's used in the overlap-add
    vec Phi; //The synthesized phase
	vec PhiPrevious;
	cx_vec Xs; //The synthesized spectrum, with modulus Xa_abs and phase Phi
	fftwf_complex *fXs; //fftw version of Xs
	fftwf_plan p2; //FFTW plan for the IFFT of fXs
	float *q; //windowed IFFT of fXs
	double *ysaida; //Overlap-add vector (time-stretched signal)
	double *ysaida2; //Pointer that points to the elemente that is equivalent to the first element of frames
	double *yshift; //The first hops[Qcolumn] elemements of ysaida2 resampled to hopa elements   
};

int nBuffersSW(uint32_t n_samples, int c64, int c128, int c256, int c_default);
float InputAbsSum(float *in, uint32_t n_samples);
