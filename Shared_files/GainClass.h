#include <cmath>
#include <stdlib.h>
#include <stdint.h>

class GainClass
{
public:
	GainClass(uint32_t n_samples);
    ~GainClass();
    void SimpleGain(double *in, double *out);
    void SimpleGain(float  *in, float  *out);
    void SimpleGain(float  *in, double *out);
    void SimpleGain(double *in, float  *out);
    void SetGaindB(double gdB);

    int N;
    double g;
    double g_1;

};