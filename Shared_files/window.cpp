#include <complex>
#include <cmath>
#include "window.h"
#include <armadillo>

using namespace arma;

void hann(int n, vec *w)
{
	vec I(n);
	I = linspace( 0, n-1, n);
	I = 0.5*(1-cos(2*M_PI*I/(n-1)));
	w[0] = I;
}
