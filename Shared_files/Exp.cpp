#include <complex>
#include <cmath>
#include "Exponencial.h"

using namespace std;

complex<double> ExponencialComplexa( double x)
{
	
	double aux;
	
	if ( (x>EXP_fim)||(x<EXP_inicio))
	{
		aux = ((x +M_PI)/(2*M_PI));
        aux = floor(aux);
        x = x-aux*(2*M_PI);
	}
	
	int n;
	n = round(((x-EXP_inicio)/(EXP_fim-EXP_inicio))*EXP_N);
	
	complex<double> Expo = Exponencial[n] ;
	
	return Expo;
}
