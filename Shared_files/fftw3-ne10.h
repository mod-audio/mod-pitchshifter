/**/

#ifndef _NE10_FFTW3_H_INCLUDED
#define _NE10_FFTW3_H_INCLUDED

#include <NE10.h>

#include <stdio.h>

#define FFTW_ESTIMATE    0
#define FFTW_WISDOM_ONLY 0

typedef float fftwf_complex[2];

typedef enum {
    fftwf_ne10_plan_r2c_1d_32,
    fftwf_ne10_plan_c2r_1d_32
} fftwf_ne10_plan_type;

typedef struct {
    fftwf_ne10_plan_type type;
    union {
        ne10_fft_r2c_cfg_float32_t r2c32;
    };
    float* real;
    union {
        /* NOTE: fftwf_complex is an array, while ne10_fft_cpx_float32_t is a struct.
                 they're still the same size though */
        fftwf_complex* complex_fftw;
        ne10_fft_cpx_float32_t* complex_ne10;
    };
} fftwf_plan_s;

typedef fftwf_plan_s* fftwf_plan;

static inline
int fftwf_import_wisdom_from_filename(const char* filename)
{
    printf("NE10 fftwf_import_wisdom_from_filename called\n");
    return 1; (void) filename;
}

static inline
float* fftwf_alloc_real(size_t n)
{
    return (float*) NE10_MALLOC (n * sizeof (ne10_float32_t));
}

static inline
fftwf_complex* fftwf_alloc_complex(size_t n)
{
    return (fftwf_complex*) NE10_MALLOC (n * sizeof (ne10_fft_cpx_float32_t));
}

static inline
fftwf_plan fftwf_plan_dft_r2c_1d(int n, float* in, fftwf_complex* out, unsigned flags)
{
    fftwf_plan_s* const plan = (fftwf_plan_s*)malloc(sizeof(fftwf_plan_s));

    if (!plan) {
        return 0;
    }

    plan->type  = fftwf_ne10_plan_r2c_1d_32;
    plan->r2c32 = ne10_fft_alloc_r2c_float32(n);
    plan->real  = in;
    plan->complex_fftw = out;

    return plan;
}

static inline
fftwf_plan fftwf_plan_dft_c2r_1d(int n, fftwf_complex* in, float* out, unsigned flags)
{
    fftwf_plan_s* const plan = (fftwf_plan_s*)malloc(sizeof(fftwf_plan_s));

    if (!plan) {
        return 0;
    }

    plan->type  = fftwf_ne10_plan_c2r_1d_32;
    plan->r2c32 = ne10_fft_alloc_r2c_float32(n);
    plan->real  = out;
    plan->complex_fftw = in;

    return plan;
}

static inline
void fftwf_execute(const fftwf_plan p)
{
    switch (p->type)
    {
    case fftwf_ne10_plan_r2c_1d_32:
        ne10_fft_r2c_1d_float32_c(p->complex_ne10, p->real, p->r2c32);
        break;
    case fftwf_ne10_plan_c2r_1d_32:
        ne10_fft_c2r_1d_float32_c(p->real, p->complex_ne10, p->r2c32);
        break;
    }
}

static inline
void fftwf_destroy_plan(fftwf_plan p)
{
    NE10_FREE(p->r2c32);
    free(p);
}

static inline
void fftwf_free(void* p)
{
    NE10_FREE(p);
}

#endif
