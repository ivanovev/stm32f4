
#ifndef __RNG_H__
#define __RNG_H__

#include <main.h>

void        rng_init(void);
void        rng_deinit(void);
uint32_t    rng_uint32(void);
uint16_t    rng_uint16(void);
void        rng_macaddr(uint8_t *macaddr);

#endif

