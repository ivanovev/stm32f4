
#ifndef __MYTCP_H__
#define __MYTCP_H__

#include "myip.h"

#if 1
#define TCP_CON_CLOSED      0x0
#define TCP_CON_LISTEN      0x1
#define TCP_CON_ESTABLISHED 0x2
#endif

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

    uint8_t *data;
} TCP_FRAME;

typedef struct {
    uint32_t remote_ip_addr;

    uint16_t local_port;
    uint16_t remote_port;

    uint32_t rcv_nxt;
    uint32_t snd_nxt;

    uint16_t len;
    uint16_t mss;

    uint8_t state;

#if 0
    u16_t initialmss;   /**< Initial maximum segment size for the
                          connection. */
    u8_t sa;            /**< Retransmission time-out calculation state
                          variable. */
    u8_t sv;            /**< Retransmission time-out calculation state
                          variable. */
    u8_t rto;           /**< Retransmission time-out. */
    u8_t tcpstateflags; /**< TCP state and flags. */
    u8_t timer;         /**< The retransmission timer. */
    u8_t nrtx;          /**< The number of retransmissions for the last
                          segment sent. */
#endif
} TCP_CONN;

void        myip_tcp_init(void);
uint16_t    myip_handle_tcp_frame(ETH_FRAME *frm, uint16_t sz);

#endif

