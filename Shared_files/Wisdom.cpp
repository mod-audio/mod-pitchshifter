#include "Wisdom.h"

#include <cstdio>
#include <cstddef>

bool ImportWisdom(const char* wisdomFile)
{
	bool have = false;

	if (fftwf_import_system_wisdom() != 0)
	{
		printf("mod-pitchshifter: using system fftw wisdom\n");
		have = true;
	}

	if (fftwf_import_wisdom_from_filename(wisdomFile) != 0)
	{
		printf("mod-pitchshifter: using wisdom file '%s'\n", wisdomFile);
		have = true;
	}

	if (!have)
		printf("mod-pitchshifter: no fftw wisdom found ('%s'), plans will be estimated\n", wisdomFile);

	return have;
}

fftwf_plan PlanR2C(int n, float* in, fftwf_complex* out, bool haveWisdom)
{
	fftwf_plan p = NULL;
	if (haveWisdom)
		p = fftwf_plan_dft_r2c_1d(n, in, out, FFTW_WISDOM_ONLY|FFTW_ESTIMATE);
	if (p == NULL)
		p = fftwf_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
	return p;
}

fftwf_plan PlanC2R(int n, fftwf_complex* in, float* out, bool haveWisdom)
{
	fftwf_plan p = NULL;
	if (haveWisdom)
		p = fftwf_plan_dft_c2r_1d(n, in, out, FFTW_WISDOM_ONLY|FFTW_ESTIMATE);
	if (p == NULL)
		p = fftwf_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
	return p;
}
