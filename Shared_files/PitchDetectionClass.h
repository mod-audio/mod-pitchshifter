#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <complex>
#include "fftw3-ne10.h"
#include <armadillo>
#include "Exp.h"
#include "angle.h"

using namespace arma;
using namespace std;

class PitchDetection
{
public:
    PitchDetection(uint32_t n_samples, int nBuffers, double SampleRate, const char* wisdomFile);
    ~PitchDetection();
    void PreProcessing(int nBuffers, float *in);
    void FindNote();

    int N; //Size of the frame
    int hopa; //Analysis hop

    float **b;
    
    float *frames; //A frame of last N samples
    fftwf_plan p; //FFTW plan for the FFT of frames
    fftwf_complex *fXa; // FFT of frames
    cx_vec Xa; // Armadillo vector of fXa
	cx_vec Xs; // Xa * conj(Xa)
	fftwf_complex *fXs; // FFTW vector of fXs
	fftwf_plan p2; //FFTW plan for the IFFT of Xs
	float *q; //IFFT of Xs
	vec R; //Autocorrelation of frames
	vec NORM; //The norm of R
	vec F; //Bias function
	vec AUTO; //Normalized-biased autocorrelation of frames
	double f; //Frequency of the note
	int note; //Note code from 0 to 11
	int oitava; //Octave number
	float fs; //Sample rate

	int Tone;
	int Scale;
	int Interval;
	int Mode;
	int LowNote;
};