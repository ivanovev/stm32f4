
#include "dsp/dsp.h"

void* __wrap_memset(void *s, int c, size_t n)
{
    return mymemset(s, c, n);
}

char* __wrap_memclr(char *s, size_t n)
{
    return (char*)mymemset(s, 0, n);
}

static arm_fir_instance_q15 Sq;

static q15_t fir_taps[DSP_NUM_TAPS] = {0x7FFF, 0, 0, 0};
static q15_t fir_state[DSP_BLOCK_SIZE + DSP_NUM_TAPS - 1];

dsp_io_func dsp_io = 0;

static uint16_t dsp_copy(uint8_t *in, uint16_t sz, uint8_t *out)
{
    mymemcpy((void*)out, (void*)in, (uint32_t)sz);
    return sz;
}

void dsp_init(void)
{
#if 0
    dsp_io = dsp_copy;
#else
    arm_status status;
    arm_fir_init_q15(&Sq, DSP_NUM_TAPS, fir_taps, fir_state, DSP_BLOCK_SIZE);
    dsp_io = dsp_fir_q15;
#endif
}

uint16_t dsp_fir_q15(uint8_t *in, uint16_t sz, uint8_t *out)
{
    q15_t *inq = (q15_t*)in, *outq = (q15_t*)out;
    arm_fir_q15(&Sq, inq, outq, sz/2);
    return sz;
}

