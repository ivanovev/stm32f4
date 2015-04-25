
#ifndef __DBG_H__
#define __DBG_H__

#include "myip.h"

#define DBG_PORT        12345

uint16_t    myip_dbg_con_handler(uint8_t *in, uint16_t sz, uint8_t *out);

#endif

