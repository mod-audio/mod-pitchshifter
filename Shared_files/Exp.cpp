#include <complex>
#include <cmath>
#include "Sin.h"
#include "Cos.h"

using namespace std;

complex<double> ExponencialComplexa( double x)
{	
	double aux;
	
	if ( (x>SIN_fim)||(x<SIN_inicio))
	{
		aux = ((x +M_PI)/(2*M_PI));
        aux = floor(aux);
        x = x-aux*(2*M_PI);
	}
	
	int n;
	n = round((x-SIN_inicio)*SIN_Idx);
	return complex<double>(Cos[n],Sin[n]);
}