#include <complex>
#include <cmath>
#include "angle.h"
#include "arc.h"

using namespace std;

double arctg( double x)
{
	int flag = 1;
	
	if (x < 0)
	{
		flag = -1;
		x = -x;
	}
	
	int n = round(((x-ATAN_inicio)/(ATAN_fim-ATAN_inicio))*ATAN_N);
	
	double ArcTg;
	
	if( x > ATAN_fim)
	{
		ArcTg = flag*M_PI/2;
	}
	else
	{
		ArcTg = Arctg[n]*flag;
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
