
#ifndef __STREAM_H__
#define __STREAM_H__

#include "myip.h"

#define STREAM_PORT        1234

#define STREAM_IN   (1 << 0)
#define STREAM_OUT  (1 << 1)
#define STREAM_IO   (STREAM_IN | STREAM_OUT)

void        myip_stream_init(void);
void        myip_stream_start(uint8_t dir);
void        myip_stream_stop(uint8_t dir);
uint8_t     myip_stream_status(void);
uint16_t    myip_stream_con_handler(uint8_t *in, uint16_t sz, uint8_t *out);
uint16_t    myip_stream_frm_handler(ethfrm_t *in, uint16_t sz, uint16_t con_index, ethfrm_t *out);

#endif

