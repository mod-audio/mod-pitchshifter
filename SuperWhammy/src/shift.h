#include <complex>
#include <cmath>
#include <fftw3.h>
#include <armadillo>

using namespace arma;
using namespace std;
void shift(int N, int hopa, int *hops, double *frames, vec *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, vec *PhiPrevious, mat *Q, double *yshift, cx_vec *Xa, cx_vec *Xs, vec *q, cx_vec *qaux, vec *Phi, double *ysaida, double *ysaida2, int Qcolumn, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftw_plan p, fftw_plan p2);
