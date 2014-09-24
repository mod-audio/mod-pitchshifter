#include <cmath>
#include "RealExponencial.h"

using namespace std;

double ExponencialReal( double x)
{	
	if ( (x>R_EXP_fim))
	{
		return RealExponencial[R_EXP_N - 1];
	}
	else
	{
		if (x<R_EXP_inicio)
		{
			return RealExponencial[0];
		}
	}
	
	int n = round(((x-R_EXP_inicio)/(R_EXP_fim-R_EXP_inicio))*R_EXP_N);
	
	return RealExponencial[n];
}
