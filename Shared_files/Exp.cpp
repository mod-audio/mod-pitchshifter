#include <complex>
#include <cmath>
#include "Exponencial.h"

using namespace std;

complex<double> ExponencialComplexa( double x)
{
	
	int d = 50;
	double inicio = -M_PI;
	double fim = M_PI;
	double aux;
	
	if ( (x>fim)||(x<inicio))
	{
		aux = ((x +M_PI)/(2*M_PI));
        aux = floor(aux);
        x = x-aux*(2*M_PI);
	}
	
	double n;
	n = ((x-inicio)/(fim-inicio))*2*d;
	int n1 = floor(n);
	int n2 = ceil(n);
	
	complex<double> Expo;
	complex<double> Expo1 = Exponencial[n1];
	complex<double> Expo2 = Exponencial[n2];
	//complex<double> j;
	//j = complex<double>(0.0, 1.0);
	//Expo = (j*(complex<double>)x);
	
	Expo = Expo1 + (Expo2-Expo1)*(n-n1);
	
	return Expo;
}
