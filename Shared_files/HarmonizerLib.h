#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <armadillo>
#include "Exp.h"
#include "angle.h"

void FindStep  (int note, int oitava, int Tone, int Scale, int Interval, int Mode, int LowNote, double *s);
void FindStepCS(int note, int oitava, int Tone, int LowNote, int s_0, int s_1, int s_2, int s_3, int s_4, int s_5, int s_6, int s_7, int s_8, int s_9, int s_10, int s_11, double *s);