
#ifndef __DSP_H__
#define __DSP_H__

#define DSP_BLOCK_SIZE            256
#define DSP_NUM_TAPS              4

#include <main.h>
#include "arm_math.h"

typedef uint16_t (*dsp_io_func)(uint8_t *in, uint16_t sz, uint8_t *out);

void            dsp_init(void);
extern          dsp_io_func dsp_io;
uint16_t        dsp_fir_q15(uint8_t *in, uint16_t sz, uint8_t *out);

#endif
