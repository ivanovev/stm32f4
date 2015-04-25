
#ifndef __TELNETD_H__
#define __TELNETD_H__

#include "myip.h"
#include "mytcp.h"
#include "util/queue.h"

#define TELNET_PORT     23

void        myip_telnetd_init(void);
uint16_t    myip_telnetd_con_handler(uint8_t *in, uint16_t sz, uint8_t *out);

uint16_t    telnetd_recv_str(char *buf);
void        telnetd_send_str(const char *str, uint16_t len);

#endif

