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
	
	int n = round((x-ATAN_inicio)*ATAN_Idx);
		
	if( x > ATAN_fim)
	{
		return flag*M_PI/2;
	}
	else
	{
		return Arctg[n]*flag;
	}	
}

double angle( complex<double> z)
{
	double x = real(z);
	double y = imag(z);
	double Angle = 0;

	if (x != 0 || y != 0)
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
					if ( y>0 )
					{
						Angle = arctg(y/x) + M_PI;
					}
					else
					{
						if ( y<0 )
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
