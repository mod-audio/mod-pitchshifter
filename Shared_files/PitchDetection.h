#include <complex>
#include <cmath>
#include <fftw3.h>
#include <armadillo>

using namespace arma;
using namespace std;
void FindNote(int N, double *frames, float *frames3, cx_vec *Xa2, cx_vec *Xs2, float *q2, int Qcolumn, fftwf_plan p3, fftwf_plan p4, fftwf_complex *fXa2, fftwf_complex *fXs2, vec *R, vec *NORM, vec *F, vec *AUTO, float fs, int *note, int *oitava);
void FindStep(int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, int hopa, int Qcolumn, double *s, int *hops);
void FindStepCS(int note, int oitava, int Tone, int s_0, int s_1, int s_2, int s_3, int s_4, int s_5, int s_6, int s_7, int s_8, int s_9, int s_10, int s_11,  int LowNote, int hopa, int Qcolumn, double *s, int *hops);
