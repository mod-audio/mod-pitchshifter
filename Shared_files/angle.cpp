#include <complex>
#include <cmath>
#include "angle.h"

using namespace std;

void angle( complex<double> z, double * target)
{
	double x = real(z);
	double y = imag(z);
	double angle = 0;

	double abs_y = abs(y)+1e-10; //to prevent 0/0

	if (x>=0)
	{
		double r = (x - abs_y) / (x + abs_y);
		angle = M_PI_4 - M_PI_4 * r;
	}
	else
	{
		double r = (x + abs_y) / (abs_y - x);
		angle = THREEPIOVERFOUR - M_PI_4 * r;
	}
	if (y<0)
	{
		*target = -angle;
		return;
	}
	else
	{
		*target = angle;
		return;
	}
	
	*target = angle;
	return;
}
