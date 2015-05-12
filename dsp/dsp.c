
#include "dsp/dsp.h"

#pragma message "DSP_BLOCK_SZ: " STR(DSP_BLOCK_SZ)
#pragma message "DSP_NUM_TAPS: " STR(DSP_NUM_TAPS)

void* __wrap_memset(void *s, int c, size_t n)
{
    return mymemset(s, c, n);
}

char* __wrap_memclr(char *s, size_t n)
{
    return (char*)mymemset(s, 0, n);
}

static arm_fir_instance_q15 Sq;

//static q15_t fir_taps[DSP_NUM_TAPS] = {0x7FFF, 0, 0, 0};
//static q15_t fir_taps[DSP_NUM_TAPS] = {37,-128,-525,-667,687,4080,8012,9779,8012,4080,687,-667,-525,-128,37,0};
static q15_t fir_taps[DSP_NUM_TAPS] = {31,62,50,-39,-177,-218,0,421,668,289,-730,-1655,-1323,988,4773,8341,9804,8341,4773,988,-1323,-1655,-730,289,668,421,0,-218,-177,-39,50,62,31,0};
static q15_t fir_state[DSP_BLOCK_SZ + DSP_NUM_TAPS - 1];

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
    arm_fir_init_q15(&Sq, DSP_NUM_TAPS, fir_taps, fir_state, DSP_BLOCK_SZ);
    dsp_io = dsp_convert_q15_unsigned2signed;
#endif
}

uint16_t dsp_fir_q15(uint8_t *in, uint16_t sz, uint8_t *out)
{
    q15_t *inq = (q15_t*)in, *outq = (q15_t*)out;
    arm_fir_q15(&Sq, inq, outq, sz/2);
    return sz;
}

uint16_t dsp_convert_q15_unsigned2signed(uint8_t *in, uint16_t sz, uint8_t *out)
{
    q15_t *inq = (q15_t*)in, *outq = (q15_t*)out;
    q15_t tmp[DSP_BLOCK_SZ];
    arm_offset_q15(inq, 0x0800, tmp, DSP_BLOCK_SZ);
    arm_shift_q15(tmp, 4, outq, DSP_BLOCK_SZ);
    return sz;
}

uint16_t dsp_offset_q15_0x0800(uint8_t *in, uint16_t sz, uint8_t *out)
{
    q15_t *inq = (q15_t*)in, *outq = (q15_t*)out;
    arm_offset_q15(inq, 0x0800, outq, DSP_BLOCK_SZ);
    return sz;
}

uint16_t dsp_shift_q15_4(uint8_t *in, uint16_t sz, uint8_t *out)
{
    q15_t *inq = (q15_t*)in, *outq = (q15_t*)out;
    arm_shift_q15(inq, 4, outq, DSP_BLOCK_SZ);
    return sz;
}

