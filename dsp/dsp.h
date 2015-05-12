
#ifndef __DSP_H__
#define __DSP_H__

#define DSP_BLOCK_SZ            IO_BUF_SZ/sizeof(q15_t)
#define DSP_NUM_TAPS            34

#include <main.h>
#include "arm_math.h"

typedef uint16_t (*dsp_io_func)(uint8_t *in, uint16_t sz, uint8_t *out);

void            dsp_init(void);
extern          dsp_io_func dsp_io;
uint16_t        dsp_fir_q15(uint8_t *in, uint16_t sz, uint8_t *out);
uint16_t        dsp_convert_q15_unsigned2signed(uint8_t *in, uint16_t sz, uint8_t *out);
uint16_t        dsp_offset_q15_0x0800(uint8_t *in, uint16_t sz, uint8_t *out);
uint16_t        dsp_shift_q15_4(uint8_t *in, uint16_t sz, uint8_t *out);

#endif
