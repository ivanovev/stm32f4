
#ifndef __ICMP_H__
#define __ICMP_H__

#include <main.h>

typedef struct
{
    uint8_t type;
    uint8_t code;
    uint16_t cksum;
    uint8_t rest[4];
} icmphdr_t;

typedef struct {
    machdr_t mac;
    iphdr_t ip;
    icmphdr_t icmp;
} icmpfrm_t;

#define ICMP_ECHO_REQUEST   0x8
#define ICMP_ECHO_REPLY     0x0

#define ICMPH_SZ    sizeof(icmphdr_t)

void        myip_icmp_init(void);
uint16_t    myip_icmp_con_handler(uint8_t *data, uint16_t sz);
uint16_t    myip_icmp_frm_handler(ethfrm_t *frm, uint16_t sz, uint16_t con_index);
uint16_t    myip_icmp_ping(uint8_t *ipaddr);

#endif

