#include <cmath>
#include <iostream>
#include <complex>
#include <stdlib.h>
#include <stdio.h>
#include <fftw3.h>
#include "arc.h"
#include "arctg.h"

using namespace std;

double arctg( double x)
{
	int d = 200000;
	double inicio = 0;
	double fim = 25000;
	int flag = 1;
	
	if (x < 0)
	{
		flag = -1;
		x = -x;
	}
	
	double n;
	n = ((x-inicio)/(fim-inicio))*2*d;
	int n1 = floor(n);
	int n2 = ceil(n);
	
	double ArcTg;
	double ArcTg1 = Arctg[n1];
	double ArcTg2 = Arctg[n2];
	
	ArcTg = (ArcTg1 + (ArcTg2-ArcTg1)*(n-n1))*flag;
	
	return ArcTg;
}
