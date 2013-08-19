#include <complex>
#include <cmath>
#include "angle.h"
#include "arc.h"

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
	double ArcTg1;
	double ArcTg2;
	
	if( x > fim)
	{
		ArcTg = flag*M_PI/2;
	}
	else
	{
		ArcTg1 = Arctg[n1];
		ArcTg2 = Arctg[n2];
		ArcTg = (ArcTg1 + (ArcTg2-ArcTg1)*(n-n1))*flag;
	}
	//ArcTg = flag*atan(x);
	
	return ArcTg;
}

double angle( complex<double> z)
{
	double x = real(z);
	double y = imag(z);
	double Angle;
	
	
	if( (x == 0)&&(y == 0) )
	{
		Angle = 0;
	}
	else
	{
		if( (x == 0)&&(y > 0) )
		{
			Angle = M_PI/2;
		}
		else
		{
			if( (x == 0)&&(y < 0) )
			{
				Angle = -M_PI/2;
			}
			else
			{
				if (x > 0)
				{
					Angle = arctg(y/x);
				}
				else
				{
					if ( (x < 0)&&(y>0) )
					{
						Angle = arctg(y/x) + M_PI;
					}
					else
					{
						if ( (x < 0)&&(y<0) )
						{
							Angle = arctg(y/x) - M_PI;
						}
					}
				}
			}
		}
	}
	
	
	
	return Angle;
}
