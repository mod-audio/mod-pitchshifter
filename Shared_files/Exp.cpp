#include <complex>
#include <cmath>
#include "Exp.h"

using namespace std;

void ExponencialComplexa( double x, complex<double> * target)
{	
	double k;

	k = ((x + M_PI) * ONEOVERTWOPI);
	x = x - ((int)(k)) * TWOPI; 
	
	//calculate sin(x) using parabola approximation
	double sin = FOUROVERPI * x + NEGFOUROVERPISQ * x * abs(x);

	//calculate cos(x) using parabola approximation on conditioned x
	x = x + M_PI_2;
	if (x > M_PI)
	{
		x = x - TWOPI;
	}
	double cos = FOUROVERPI * x + NEGFOUROVERPISQ * x * abs(x);

	*target = complex<double>(cos, sin);
}

