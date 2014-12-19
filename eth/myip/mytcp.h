
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
    uint16_t src_port;
    uint16_t dst_port;

    uint32_t seqn;
    uint32_t ackn;

    uint8_t offset;
    uint8_t flags;
    uint16_t wndsz;

    uint16_t cksum;
    uint16_t urg;
} tcphdr_t;

#define TCPH_SZ     sizeof(tcphdr_t)

typedef struct {
    machdr_t mac;
    iphdr_t ip;
    tcphdr_t tcp;
    uint8_t data[];
} tcpfrm_t;

void        myip_tcp_init(void);
void        myip_tcp_con_close(void);
uint16_t    myip_tcp_frm_handler(ethfrm_t *frm, uint16_t sz, uint16_t con_index);
uint16_t    myip_tcp_con_closed(void);

#endif

