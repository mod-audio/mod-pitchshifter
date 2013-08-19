#include <complex>
#include <cmath>
#include <fftw3.h>
using namespace std;
void shift(int N, int hopa, int *hops, double *frames, double *w, complex<double> *XaPrevious, double *XaPrevious_arg, double *PhiPrevious, double **Q, double *yshift, complex<double> *Xa, complex<double> *Xs, double *q, complex<double> *qaux, complex<double> *framesaux, double *Phi, double *ysaida, double *ysaida2, int Qcolumn, fftw_plan p, fftw_plan p2);
