#include "GainClass.h"

GainClass::GainClass(uint32_t n_samples) //Constructor
{
	N = (int)n_samples;
	g = 0;
	g_1 = 0;
}

GainClass::~GainClass(){} //Destructor

void GainClass::SetGaindB(double gdB)
{
	g = pow(10, gdB/20.0);
}

void GainClass::SimpleGain(double *in, double *out)
{
	for (int i=0; i<N; i++) out[i] = (g_1 + ((g - g_1)/(N - 1))*i )*in[i];
	g_1 = g;
}

void GainClass::SimpleGain(float *in, float *out)
{
	for (int i=0; i<N; i++) out[i] = (g_1 + ((g - g_1)/(N - 1))*i )*in[i];
	g_1 = g;
}

void GainClass::SimpleGain(double *in, float *out)
{
	for (int i=0; i<N; i++) out[i] = (g_1 + ((g - g_1)/(N - 1))*i )*in[i];
	g_1 = g;
}

void GainClass::SimpleGain(float *in, double *out)
{
	for (int i=0; i<N; i++) out[i] = (g_1 + ((g - g_1)/(N - 1))*i )*in[i];
	g_1 = g;
}

