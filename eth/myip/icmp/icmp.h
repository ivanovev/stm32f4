
#ifndef __ICMP_H__
#define __ICMP_H__

#define ICMP_ECHO_REQUEST   0x8
#define ICMP_ECHO_REPLY     0x0

#include <main.h>

void        myip_icmp_init(void);
uint16_t    myip_icmp_con_handler(uint8_t *data, uint16_t sz);
uint16_t    myip_icmp_frm_handler(ETH_FRAME *frm, uint16_t sz, uint16_t con_index);
uint16_t    myip_icmp_ping(uint8_t *ipaddr);

#endif

