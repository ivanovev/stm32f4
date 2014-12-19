
#ifndef __MYIP_H__
#define __MYIP_H__

#include <main.h>

#pragma pack(1)
typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t type;
} machdr_t;

typedef struct
{
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t src_mac_addr[6];
    uint8_t src_ip_addr[4];
    uint8_t dst_mac_addr[6];
    uint8_t dst_ip_addr[4];
} arphdr_t;

typedef struct
{
    uint8_t ver_ihl;
    uint8_t dscp_ecn;
    uint16_t total_len;
    uint16_t id;
    uint16_t frag;
    uint8_t ttl;
    uint8_t proto;
    uint16_t header_cksum;

    uint8_t src_ip_addr[4];
    uint8_t dst_ip_addr[4];
} iphdr_t;

typedef struct
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t cksum;
} udphdr_t;

typedef union
{
    uint8_t packet[ETH_MAX_PACKET_SIZE];
    struct
    {
        machdr_t mac;
        uint8_t data[];
    } e;
} ethfrm_t;

typedef struct
{
    machdr_t mac;
    arphdr_t arp;
} arpfrm_t;

typedef struct
{
    machdr_t mac;
    iphdr_t ip;
    uint8_t data[];
} ipfrm_t;

typedef struct {
    machdr_t mac;
    iphdr_t ip;
    udphdr_t udp;
    uint8_t data[];
} udpfrm_t;

typedef uint16_t (*frm_handler)(ethfrm_t *frm, uint16_t sz, uint16_t con_index);
typedef uint16_t (*con_handler)(uint8_t* data, uint16_t sz);

typedef struct {
    uint16_t port;
    uint8_t proto;
    uint8_t state;
    frm_handler frm_handler_ptr;
    con_handler con_handler_ptr;
} CON_ENTRY;

#if 0
#define MACH_SZ     ETH_HEADER
#define ARPH_SZ     28
#define IPH_SZ      20
#define UDPH_SZ     8
#define TCPH_SZ     20
#define ICMPH_SZ    8
#else
#define MACH_SZ     sizeof(machdr_t)
#define ARPH_SZ     sizeof(arphdr_t)
#define IPH_SZ      sizeof(iphdr_t)
#define UDPH_SZ     sizeof(udphdr_t)
#endif

typedef struct
{
    uint8_t ip_addr[4];
    uint8_t mac_addr[6];
    uint32_t time;
} ARP_ENTRY;

#define ARP_TABLE_SZ        10
#define CON_TABLE_SZ        10

#define ARP_OPER_REQ        0x0100
#define ARP_OPER_REPL       0x0200

#define arpfrm_t_TYPE  0x0608
#define ipfrm_t_TYPE   0x0008

#define ICMP_PROTO          0x01
#define TCP_PROTO           0x06
#define UDP_PROTO           0x11

void        myip_init(void);
void        myip_con_add(frm_handler frm_handler_ptr, con_handler con_handler_ptr, uint8_t proto, uint16_t port);
uint16_t    myip_arp_find(const uint8_t *ip_addr);

uint16_t    myip_eth_frm_handler(ethfrm_t *frm, uint16_t sz);
uint16_t    myip_arp_frm_handler(ethfrm_t *frm, uint16_t sz);
uint16_t    myip_udp_frm_handler(ethfrm_t *frm, uint16_t sz, uint16_t con_index);

void        myip_make_ip_frame(ipfrm_t *ifrm, const uint8_t *dst_ip_addr, uint16_t hsz, uint16_t proto);
void        myip_make_arp_frame(arpfrm_t *afrm, uint8_t *dst_ip_addr, uint16_t oper);
void        myip_make_udp_frame(udpfrm_t *ufrm, const uint8_t *dst_ip_addr, uint16_t src_port, uint16_t dst_port, uint16_t data_sz);

#endif

