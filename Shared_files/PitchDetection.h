#include <complex>
#include <cmath>
#include <fftw3.h>
#include <armadillo>

using namespace arma;
using namespace std;
void FindNote(int N, double *frames, float *frames3, cx_vec *Xa2, cx_vec *Xs2, float *q2, int Qcolumn, fftwf_plan p3, fftwf_plan p4, fftwf_complex *fXa2, fftwf_complex *fXs2, vec *R, vec *NORM, vec *F, vec *AUTO, float fs, int *note, int *oitava);
void FindStep(int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, int hopa, int Qcolumn, double *s, int *hops);
