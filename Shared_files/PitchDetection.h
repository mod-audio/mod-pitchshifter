#include <complex>
#include <cmath>
#include <fftw3.h>
#include <armadillo>
#include <stdint.h>

using namespace arma;
using namespace std;

class PitchDetection
{
public:
    PitchDetection(uint32_t n_samples, int nBuffers);
    ~PitchDetection();
    void PreProcessing(int nBuffers, float *in);
    void FindNote();

    int N; //Size of the frame
    int hopa; //Analysis hop
    int Qcolumn; //Number of frames that may be used in the overlap-add

    float **b;
    
    float *frames; //A frame of last N samples
    fftwf_plan p; //FFTW plan for the FFT of frames
    fftwf_complex *fXa; // FFT of frames
    cx_vec *Xa; // Armadillo vector of fXa
	cx_vec *Xs; // Xa * conj(Xa)
	fftwf_complex *fXs; // FFTW vector of fXs
	fftwf_plan p2; //FFTW plan for the IFFT of Xs
	float *q; //IFFT of Xs
	vec *R; //Autocorrelation of frames
	vec *NORM; //The norm of R
	vec *F; //Bias function
	vec *AUTO; //Normalized-biased autocorrelation of frames
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

void FindNote(int N, double *frames, float *frames3, cx_vec *Xa2, cx_vec *Xs2, float *q2, int Qcolumn, fftwf_plan p3, fftwf_plan p4, fftwf_complex *fXa2, fftwf_complex *fXs2, vec *R, vec *NORM, vec *F, vec *AUTO, float fs, int *note, int *oitava);
void FindStep(int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, int hopa, int Qcolumn, double *s, int *hops);
void FindStepCS(int note, int oitava, int Tone, int s_0, int s_1, int s_2, int s_3, int s_4, int s_5, int s_6, int s_7, int s_8, int s_9, int s_10, int s_11,  int LowNote, int hopa, int Qcolumn, double *s, int *hops);
