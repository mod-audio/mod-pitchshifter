#ifndef MOD_PITCHSHIFTER_WISDOM_H
#define MOD_PITCHSHIFTER_WISDOM_H

#include <fftw3.h>

/* Load FFTW wisdom once per plugin instance: the system wisdom (if the OS
 * ships one) and then the bundle's harmonizer.wisdom, both merged into the
 * planner. Returns true when any of the two was read. Not real-time safe:
 * call from instantiate() only. */
bool ImportWisdom(const char* wisdomFile);

/* Plan a transform from wisdom when it covers this size, otherwise fall back
 * to an estimated plan. The old code planned FFTW_WISDOM_ONLY and silently got
 * a NULL plan (no transform at all) for any size the wisdom did not know.
 * Not real-time safe: call from instantiate() only. */
fftwf_plan PlanR2C(int n, float* in, fftwf_complex* out, bool haveWisdom);
fftwf_plan PlanC2R(int n, fftwf_complex* in, float* out, bool haveWisdom);

#endif
