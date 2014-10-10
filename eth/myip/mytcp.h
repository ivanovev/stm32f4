
#ifndef __MYTCP_H__
#define __MYTCP_H__

#include "myip.h"

#define TCP_CON_CLOSED      0x0
#define TCP_CON_LISTEN      0x1
#define TCP_CON_ESTABLISHED 0x2
#define TCP_CON_CLOSE       0x3

#define TCP_TABLE_SZ        5

#pragma pack(1)
typedef struct {
    preamble3 p;

    uint32_t seqn;
    uint32_t ackn;

    uint8_t offset;
    uint8_t flags;
    uint16_t wndsz;

    uint16_t cksum;
    uint16_t urg;

    uint8_t data[];
} TCP_FRAME;

typedef struct {
    volatile uint8_t state;
    uint8_t remote_mac_addr[6];
    uint32_t remote_ip_addr;
    uint16_t remote_port;
    uint16_t local_port;
    uint16_t id;

    uint32_t seqn;
    uint32_t ackn;
    uint32_t rxseqn0;
} TCP_CON;

void        myip_tcp_init(void);
uint16_t    myip_tcp_frame_handler(ETH_FRAME *frm, uint16_t sz, uint8_t con_index);
uint16_t    myip_tcp_con_handler(ETH_FRAME *frm, uint16_t sz, uint8_t con_index);
uint16_t    myip_tcp_con_closed(void);

#endif

