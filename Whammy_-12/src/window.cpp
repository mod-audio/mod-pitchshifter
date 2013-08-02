#include <complex>
#include <cmath>
#include "window.h"

void hann(int n, double *w)
{
	for (int i=1; i<=n; i++)
	{
		w[i-1] = 0.5*(1 - cos(2*M_PI*(i-1)/n));
	}
}

void hann2(int n, double *w)
{
	double wlinha[2*n+1];
	hann(2*n+1, wlinha);
	for (int i=1; i<=n; i++)
	{
		w[i-1] = wlinha[2*i-1];
	}
}
