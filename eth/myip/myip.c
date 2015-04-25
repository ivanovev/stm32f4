
#include "eth/eth.h"
#ifdef ENABLE_ICMP
#include "icmp/icmp.h"
#endif
#ifdef ENABLE_PTP
#include "ptp/ptpd.h"
#endif
#ifdef ENABLE_TELNET
#include "telnet/telnetd.h"
#endif
#ifdef ENABLE_TFTP
#include "tftp/tftpd.h"
#endif
#ifdef ENABLE_DBG
#include "dbg/dbg.h"
#endif

#define IP_VER_IHL          0x45
#define IP_TTL              0x80
#define FRAG_MASK           0xFFBF
#define FRAG_FLAG           0x0020

#define ARP_HTYPE           0x0100
#define ARP_PTYPE           0x0008
#define ARP_HLEN            0x06
#define ARP_PLEN            0x04

#ifdef ENABLE_ICMP
ARP_ENTRY arp_table[ARP_TABLE_SZ];
CON_ENTRY con_table[CON_TABLE_SZ];

uint8_t local_ipaddr[4] = {IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3};
uint8_t local_macaddr[6] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};

void myip_init(void)
{
    mymemset(&arp_table, 0, ARP_TABLE_SZ*sizeof(ARP_ENTRY));
    mymemset(&con_table, 0, CON_TABLE_SZ*sizeof(CON_ENTRY));
    myip_icmp_init();
    myip_con_add(myip_icmp_frm_handler, myip_icmp_con_handler, ICMP_PROTO, 0);
#ifdef ENABLE_TFTP
    myip_tftpd_init();
    myip_con_add(myip_udp_frm_handler, myip_tftpd_con_handler, UDP_PROTO, TFTP_PORT);
#endif
#ifdef ENABLE_PTP
    myip_ptpd_init();
    myip_con_add(myip_ptpd_frm_handler, myip_ptpd_evt_io, UDP_PROTO, PTP_EVT_PORT);
    myip_con_add(myip_ptpd_frm_handler, myip_ptpd_msg_io, UDP_PROTO, PTP_MSG_PORT);
    myip_con_add(myip_ptpd_frm_handler, 0, UDP_PROTO, 0);
#endif
#ifdef ENABLE_DBG
    myip_con_add(myip_udp_frm_handler2, myip_dbg_con_handler, UDP_PROTO, DBG_PORT);
#endif
#ifdef ENABLE_TELNET
    myip_tcp_init();
    myip_telnetd_init();
    myip_con_add(myip_tcp_frm_handler, myip_telnetd_con_handler, TCP_PROTO, TELNET_PORT);
#endif
}
#endif

#ifdef ENABLE_ICMP
void myip_con_add(frm_handler frm_handler_ptr, con_handler con_handler_ptr, uint8_t proto, uint16_t port)
{
    static uint8_t i = 0;
    con_table[i].port = port;
    con_table[i].proto = proto;
    con_table[i].frm_handler_ptr = frm_handler_ptr;
    con_table[i].con_handler_ptr = con_handler_ptr;
    i++;
}

uint16_t myip_udp_data(udpfrm_t *ufrm, uint16_t sz, uint8_t **ptr)
{
    iphdr_t *ip = &ufrm->ip;
    udphdr_t *udp = &ufrm->udp;
    uint32_t tmp2 = (uint32_t)udp->src_port;
    tmp2 += UDPH_SZ;
    *ptr = (uint8_t*)tmp2;
    if(sz)
        return HTONS_16(ip->total_len) - IPH_SZ - UDPH_SZ;
    //return ETH_MAX_PACKET_SIZE - IPH_SZ - UDPH_SZ;
    return 0;
}

uint16_t myip_arp_find(const uint8_t *ip_addr)
{
    uint16_t i;
    for(i = 0; i < ARP_TABLE_SZ; i++)
    {
        if(!mymemcmp(arp_table[i].ip_addr, ip_addr, 4))
            break;
    }
    return i;
}

uint16_t myip_con_find(uint16_t port, uint8_t proto)
{
    uint16_t i;
    for(i = 0; i < CON_TABLE_SZ; i++)
    {
        if((con_table[i].port == port) && (con_table[i].proto == proto))
            break;
    }
    return i;
}

void myip_update_mac_hdr(machdr_t *mac, const uint8_t *dst_ip_addr)
{
    uint16_t i = myip_arp_find(dst_ip_addr);
    mymemcpy(mac->src, local_macaddr, 6);
    if(i != ARP_TABLE_SZ)
        mymemcpy(mac->dst, arp_table[i].mac_addr, 6);
    mac->type = ipfrm_t_TYPE;
}

void myip_update_ip_hdr(iphdr_t *ip, const uint8_t *dst_ip_addr, uint16_t sz, uint16_t proto)
{
    static uint16_t ip_counter = 0;
    ip->ver_ihl = IP_VER_IHL;
    ip->dscp_ecn = 0x00;
    ip->total_len = HTONS_16(IPH_SZ + sz);
    //ip->id = HTONS_16(id);
    ip->id = HTONS_16(ip_counter);
    ip_counter++;
    ip->frag = HTONS_16(0x4000);
    ip->ttl = IP_TTL;
    ip->proto = proto;
    ip->header_cksum = 0;
    mymemcpy(ip->dst_ip_addr, dst_ip_addr, 4);
    mymemcpy(ip->src_ip_addr, local_ipaddr, 4);
}

void myip_update_udp_hdr(udphdr_t *udp, uint16_t dst_port, uint16_t src_port, uint16_t sz)
{
    udp->dst_port = dst_port;
    udp->src_port = src_port;
    udp->len = HTONS_16(UDPH_SZ + sz);
    udp->cksum = 0;
}

void myip_update_arp_hdr(arphdr_t *arp, uint8_t *dst_ip_addr, uint16_t oper)
{
    arp->htype = ARP_HTYPE;
    arp->ptype = ARP_PTYPE;
    arp->hlen = ARP_HLEN;
    arp->plen = ARP_PLEN;
    arp->oper = oper;
    mymemcpy(arp->dst_ip_addr, dst_ip_addr, 4);
    uint16_t i = myip_arp_find(dst_ip_addr);
    if(i != ARP_TABLE_SZ)
        mymemcpy(arp->dst_mac_addr, arp_table[i].mac_addr, 6);
    else
        mymemset(arp->dst_mac_addr, 0, 6);
    mymemcpy(arp->src_ip_addr, local_ipaddr, 4);
    mymemcpy(arp->src_mac_addr, local_macaddr, 6);
}

void myip_make_ip_frame(ipfrm_t *ifrm, const uint8_t *dst_ip_addr, uint16_t sz, uint16_t proto)
{
    myip_update_mac_hdr(&ifrm->mac, dst_ip_addr);
    myip_update_ip_hdr(&ifrm->ip, dst_ip_addr, sz, proto);
}

void myip_make_arp_frame(arpfrm_t *afrm, uint8_t *dst_ip_addr, uint16_t oper)
{
    mymemcpy(afrm->mac.src, local_macaddr, 6);
    mymemset(afrm->mac.dst, 0xFF, 6);
    afrm->mac.type = arpfrm_t_TYPE;
    myip_update_arp_hdr(&afrm->arp, dst_ip_addr, oper);
}

uint16_t myip_make_udp_frame(udpfrm_t *ufrm, const uint8_t *dst_ip_addr, uint16_t src_port, uint16_t dst_port, uint16_t sz)
{
    myip_update_mac_hdr(&ufrm->mac, dst_ip_addr);
    myip_update_ip_hdr(&ufrm->ip, dst_ip_addr, UDPH_SZ + sz, UDP_PROTO);
    myip_update_udp_hdr(&ufrm->udp, ufrm->udp.src_port, ufrm->udp.dst_port, sz);
    return MACH_SZ + IPH_SZ + UDPH_SZ + sz;
}

static void myip_arp_table_update(uint8_t *ip_addr, uint8_t *mac_addr)
{
    uint16_t i = myip_arp_find(ip_addr);
    if(i < ARP_TABLE_SZ)
    {
        mymemcpy(arp_table[i].ip_addr, ip_addr, 4);
        mymemcpy(arp_table[i].mac_addr, mac_addr, 6);
        arp_table[i].time = 0;
        return;
    }
    uint16_t max_time_index = 0;
    for(i = 0; i < ARP_TABLE_SZ; i++)
    {
        if(arp_table[max_time_index].time < arp_table[i].time)
            max_time_index = i;
        if (arp_table[i].time < 0xFFFF)
            arp_table[i].time += 1;
    }
    mymemcpy(arp_table[max_time_index].ip_addr, ip_addr, 4);
    mymemcpy(arp_table[max_time_index].mac_addr, mac_addr, 6);
    arp_table[max_time_index].time = 0;
}

#if 0
uint16_t myip_eth_frm_handler(ethfrm_t *frm, uint16_t sz)
{
    ipfrm_t* ipfrm = (ipfrm_t*)frm;
    if(sz)
    {
        if(frm->e.mac.type == arpfrm_t_TYPE)
            return myip_arp_frm_handler(frm, sz);

        if ((ipfrm->ip.ver_ihl != IP_VER_IHL) || ((ipfrm->ip.frag & FRAG_MASK) != 0))
            return 0;
        myip_arp_table_update(ipfrm->ip.src_ip_addr, ipfrm->mac.src);
    }
    static uint8_t i = 0;
    uint16_t sz1 = 0;
    if(sz)
        i = 0;
    if(i >= CON_TABLE_SZ)
        i = 0;
    if(!con_table[i].con_handler_ptr)
        i = 0;
    for(; i < CON_TABLE_SZ; i++)
    {
        if(!con_table[i].frm_handler_ptr)
            break;
        if(sz)
        {
            if(con_table[i].proto != ipfrm->ip.proto)
                continue;
        }
        sz1 = con_table[i].frm_handler_ptr(frm, sz, i);
        if(sz1)
            break;
    }
    return sz1;
}
#else
uint16_t myip_eth_frm_handler2(ethfrm_t *in, uint16_t sz, ethfrm_t *out)
{
    ipfrm_t* ipfrm = (ipfrm_t*)in;
    if(sz)
    {
        if(in->e.mac.type == arpfrm_t_TYPE)
            return myip_arp_frm_handler2(in, sz, out);

        if ((ipfrm->ip.ver_ihl != IP_VER_IHL) || ((ipfrm->ip.frag & FRAG_MASK) != 0))
            return 0;
        myip_arp_table_update(ipfrm->ip.src_ip_addr, ipfrm->mac.src);
    }
    static uint8_t i = 0;
    uint16_t sz1 = 0;
    if(sz)
        i = 0;
    if(i >= CON_TABLE_SZ)
        i = 0;
    if(!con_table[i].con_handler_ptr)
        i = 0;
    for(; i < CON_TABLE_SZ; i++)
    {
        if(!con_table[i].frm_handler_ptr)
            break;
        if(sz)
        {
            if(con_table[i].proto != ipfrm->ip.proto)
                continue;
        }
        sz1 = con_table[i].frm_handler_ptr(in, sz, i, out);
        if(sz1)
            break;
    }
    //if(sz1)
        //mymemcpy(out->packet, in->packet, sz1);
    return sz1;
}
#endif

#if 0
uint16_t myip_arp_frm_handler(ethfrm_t *frm, uint16_t sz)
{
    arpfrm_t *afrm = (arpfrm_t*)frm;
    arphdr_t *arp = &afrm->arp;
    if((arp->htype != ARP_HTYPE) || (arp->ptype != ARP_PTYPE) || \
      ((arp->hlen) != ARP_HLEN) || ((arp->plen) != ARP_PLEN)) {
        return 0;
    }
    if(mymemcmp(arp->dst_ip_addr, local_ipaddr, 4))
        return 0;
    myip_arp_table_update(arp->src_ip_addr, arp->src_mac_addr);
    if(arp->oper == ARP_OPER_REQ)
    {
        myip_make_arp_frame(afrm, arp->dst_ip_addr, ARP_OPER_REPL);
        return MACH_SZ + ARPH_SZ;
    }
    return 0;
}
#else
uint16_t myip_arp_frm_handler2(ethfrm_t *in, uint16_t sz, ethfrm_t *out)
{
    arpfrm_t *afrm = (arpfrm_t*)in;
    arphdr_t *arp = &afrm->arp;
    if((arp->htype != ARP_HTYPE) || (arp->ptype != ARP_PTYPE) || \
      ((arp->hlen) != ARP_HLEN) || ((arp->plen) != ARP_PLEN)) {
        return 0;
    }
    if(mymemcmp(arp->dst_ip_addr, local_ipaddr, 4))
        return 0;
    myip_arp_table_update(arp->src_ip_addr, arp->src_mac_addr);
    if(arp->oper == ARP_OPER_REQ)
    {
        myip_make_arp_frame((arpfrm_t*)out, arp->dst_ip_addr, ARP_OPER_REPL);
        return MACH_SZ + ARPH_SZ;
    }
    return 0;
}
#endif

uint16_t myip_udp_frm_handler2(ethfrm_t *in, uint16_t sz, uint16_t con_index, ethfrm_t *out)
{
    udpfrm_t* ufrmi = (udpfrm_t*)in;
    udpfrm_t* ufrmo = (udpfrm_t*)out;
    if(con_index >= CON_TABLE_SZ)
        return 0;
    if(sz)
    {
        if(ufrmi->ip.proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].port != HTONS_16(ufrmi->udp.dst_port))
            return 0;
        sz = HTONS_16(ufrmi->ip.total_len) - IPH_SZ - UDPH_SZ;
    }
    sz = con_table[con_index].con_handler_ptr(ufrmi->data, sz, ufrmo->data);
    if(sz)
    {
        return myip_make_udp_frame(ufrmo, ufrmi->ip.src_ip_addr, HTONS_16(ufrmi->udp.src_port), HTONS_16(ufrmi->udp.dst_port), sz);
        //return MACH_SZ + IPH_SZ + UDPH_SZ + sz;
    }
    return 0;
}
#else
uint16_t myip_eth_frm_handler(ethfrm_t *frm, uint16_t sz)
{
    return 0;
}
#endif

