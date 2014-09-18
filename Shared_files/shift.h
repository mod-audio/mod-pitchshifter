#include <complex>
#include <cmath>
#include <fftw3.h>
#include <armadillo>

using namespace arma;
using namespace std;
void  shift(int N, int hopa, int *hops, double *frames, float *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, vec *PhiPrevious, double *yshift, cx_vec *Xa, cx_vec *Xs, float *q, vec *Phi, double *ysaida, double *ysaida2, int Qcolumn, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftwf_plan p, fftwf_plan p2, fftwf_complex *fXa, fftwf_complex *fXs);
void shift1(int N, int hopa, double *frames, float *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, cx_vec *Xa, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftwf_plan p, fftwf_complex *fXa);
void shift2(int *hops, vec *PhiPrevious, double *yshift, cx_vec *Xs,  float *q, vec *Phi, double *ysaida, double *ysaida2, int Qcolumn, fftwf_plan p2, fftwf_complex *fXs,int N, int hopa, double *frames, float *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, cx_vec *Xa, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftwf_plan p, fftwf_complex *fXa);