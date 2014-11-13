
#ifndef __MYIP_H__
#define __MYIP_H__

#include <main.h>

#pragma pack(1)
typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t type;
} preamble1;

typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t type;

    uint8_t ver_ihl;
    uint8_t dscp_ecn;
    uint16_t total_len;
    uint16_t id;
    uint16_t frag;
    uint8_t ttl;
    uint8_t proto;
    uint16_t header_cksum;

    uint32_t src_ip_addr;
    uint32_t dst_ip_addr;
} preamble2;

typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t type;

    uint8_t ver_ihl;
    uint8_t dscp_ecn;
    uint16_t total_len;
    uint16_t id;
    uint16_t frag;
    uint8_t ttl;
    uint8_t proto;
    uint16_t header_cksum;

    uint32_t src_ip_addr;
    uint32_t dst_ip_addr;
    uint16_t src_port;
    uint16_t dst_port;
} preamble3;

typedef union
{
    uint8_t packet[ETH_MAX_PACKET_SIZE];
    struct
    {
        preamble1 p;
        uint8_t *payload;
    } e;
} ETH_FRAME;

typedef struct
{
    preamble1 p;
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t src_mac_addr[6];
    uint32_t src_ip_addr;
    uint8_t dst_mac_addr[6];
    uint32_t dst_ip_addr;
} ARP_FRAME;

typedef struct
{
    preamble2 p;

    uint8_t *data;
} IP_FRAME;

typedef struct {
    preamble2 p;

    uint8_t type;
    uint8_t code;
    uint16_t cksum;

    uint8_t *data;
} ICMP_FRAME;

typedef struct {
    preamble3 p;

    uint16_t len;
    uint16_t cksum;

    uint8_t *data;
} UDP_FRAME;

typedef uint16_t (*con_handler)(uint8_t* data, uint16_t sz);

typedef struct {
    uint16_t port;
    uint8_t proto;
    uint8_t bcast;
    uint8_t state;
    con_handler con_handler_ptr;
} CON_ENTRY;

#define MACH_SZ   ETH_HEADER
#define IPH_SZ    20
#define UDPH_SZ    8
#define TCPH_SZ   20
#define MACIPUDPH_SZ (MACH_SZ + UDPH_SZ + IPH_SZ)
#define MACIPTCPH_SZ (MACH_SZ + TCPH_SZ + IPH_SZ)

typedef struct
{
    uint32_t ip_addr;
    uint8_t mac_addr[6];
    uint32_t time;
} ARP_ENTRY;

#define ARP_TABLE_SZ        10
#define CON_TABLE_SZ        10

#define ARP_FRAME_TYPE  0x0608
#define IP_FRAME_TYPE   0x0008

#define ARP_HTYPE           0x0100
#define ARP_PTYPE           0x0008
#define ARP_HLEN            0x06
#define ARP_PLEN            0x04
#define ARP_OPER_REQ        0x0100
#define ARP_OPER_REPL       0x0200

#define HTONS_16( x ) ((((x) >> 8) + ((x) << 8)) & 0x0000FFFF)
#define HTONS_32( x ) ( ((x >> 24) & 0x000000FF) + ((x >> 8) & 0x0000FF00) + \
                        ((x << 8) & 0x00FF0000) + ((x << 24) & 0xFF000000))

#define IP_VER_IHL          0x45
#define IP_TTL              0x80
#define FRAG_MASK           0xFFBF
#define FRAG_FLAG           0x0020

#define ICMP_PROTO          0x01
#define ICMP_ECHO           0x8
#define ICMP_ECHO_REPLY     0x0

#define UDP_PROTO           0x11
#define TCP_PROTO          0x06

#define UDP_PORT_DBG        1234
#define TCP_PORT_TELNET     23
#define TCP_PORT_DATA       8888
#ifdef ENABLE_PTP
#define UDP_PORT_PTP_EVT    319
#define UDP_PORT_PTP_MSG    320
#endif
#define BCAST_REJECT    0
#define BCAST_ACCEPT    1

void        myip_init(void);
void        myip_update_arp_table(uint32_t ip_addr, uint8_t *mac_addr);
void        myip_con_add(uint16_t port, uint8_t proto, uint8_t bcast, con_handler con_handler_ptr);
uint16_t    myip_eth_frame_handler(ETH_FRAME *frm, uint16_t sz);
uint16_t    myip_arp_frame_handler(ETH_FRAME *frm, uint16_t sz);
uint16_t    myip_icmp_frame_handler(ETH_FRAME *frm, uint16_t sz);
uint16_t    myip_udp_frame_handler(ETH_FRAME *frm, uint16_t sz);
//uint16_t    myip_handle_ip_frame(ETH_FRAME *frm, uint16_t sz);
uint16_t    myip_udp_con_handler(ETH_FRAME *frm, uint16_t sz, uint8_t con_index);
void        myip_swap_addr(preamble3 *p);

#endif

