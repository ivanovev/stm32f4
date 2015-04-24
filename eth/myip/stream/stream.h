
#ifndef __STREAM_H__
#define __STREAM_H__

#include "myip.h"

#define STREAM_PORT        1234

uint16_t    myip_stream_con_handler(uint8_t *in, uint16_t sz, uint8_t *out);

#endif

