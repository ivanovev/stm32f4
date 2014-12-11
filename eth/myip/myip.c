
#include "eth.h"
#include "mydatad.h"
#ifdef ENABLE_ICMP
#include "icmp/icmp.h"
#endif
#ifdef ENABLE_PTP
#include "ptp/myptpd.h"
#endif
#ifdef ENABLE_PTP
#include "telnet/mytelnetd.h"
#endif

ARP_ENTRY arp_table[ARP_TABLE_SZ];
CON_ENTRY con_table[CON_TABLE_SZ];

uint8_t local_ipaddr[4] = {IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3};
uint8_t local_macaddr[6] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};
uint16_t ip_counter = 0;

uint16_t dbg_con_handler(uint8_t *data, uint16_t sz);

void myip_init(void)
{
    uint32_t i;
    for(i = 0; i < ARP_TABLE_SZ; i++)
    {
        mymemset(&arp_table[i], 0, sizeof(ARP_ENTRY));
    }
    for (i = 0; i < CON_TABLE_SZ; i++)
    {
        con_table[i].port = 0;
        con_table[i].state = 0;
        con_table[i].con_handler_ptr = 0;
    }
#ifdef ENABLE_ICMP
    myip_icmp_init();
    myip_con_add(myip_icmp_frm_handler, myip_icmp_con_handler, ICMP_PROTO, 0);
#endif
#ifdef ENABLE_PTP
    myip_ptpd_init();
    myip_con_add(myip_ptpd_frm_handler, myip_ptpd_evt_io, UDP_PROTO, UDP_PORT_PTP_EVT);
    myip_con_add(myip_ptpd_frm_handler, myip_ptpd_msg_io, UDP_PROTO, UDP_PORT_PTP_MSG);
    myip_con_add(myip_ptpd_frm_handler, 0, UDP_PROTO, 0);
#endif
    myip_tcp_init();
#ifdef ENABLE_TELNET
    myip_con_add(myip_tcp_frm_handler, myip_telnetd_con_handler, TCP_PROTO, TCP_PORT_TELNET);
#endif
    //myip_con_add(dbg_con_handler, UDP_PROTO);
    //myip_con_add(myip_datad_io, TCP_PROTO);
}

uint16_t dbg_con_handler(uint8_t *data, uint16_t sz)
{
    uint16_t i;
    uint32_t *ptr1, *ptr2;
    for(i = 0; i < sz; i += 4)
    {
        ptr1 = (uint32_t*)&data[i];
        ptr2 = (uint32_t*)(HTONS_32(*ptr1));
        dbg_send_hex2("addr", (uint32_t)ptr2);
        *ptr1 = HTONS_32(*ptr2);
    }
    return sz;
}

void myip_con_add(frm_handler frm_handler_ptr, con_handler con_handler_ptr, uint8_t proto, uint16_t port)
{
    static uint8_t i = 0;
    con_table[i].port = port;
    con_table[i].proto = proto;
    con_table[i].frm_handler_ptr = frm_handler_ptr;
    con_table[i].con_handler_ptr = con_handler_ptr;
    i++;
}

uint16_t myip_udp_data(UDP_FRAME *ufrm, uint16_t sz, uint8_t **ptr)
{
    uint32_t tmp2 = (uint32_t)&ufrm->p.src_port;
    tmp2 += UDPH_SZ;
    *ptr = (uint8_t*)tmp2;
    if(sz)
        return HTONS_16(ufrm->p.total_len) - IPH_SZ - UDPH_SZ;
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

void myip_update_ip_mac_addr(preamble2 *p, const uint8_t *dst_ip_addr)
{
    uint16_t i = myip_arp_find(dst_ip_addr);
    mymemcpy(p->src, local_macaddr, 6);
    mymemcpy(p->dst_ip_addr, dst_ip_addr, 4);
    mymemcpy(p->src_ip_addr, local_ipaddr, 4);
    if(i != ARP_TABLE_SZ)
        mymemcpy(p->dst, arp_table[i].mac_addr, 6);
}

void myip_make_arp_frame(ARP_FRAME *afrm, uint8_t *dst_ip_addr, uint16_t oper)
{
    mymemset(&afrm->p.dst, 0xFF, 6);
    mymemcpy(&afrm->p.src, local_macaddr, 6);
    afrm->p.type = ARP_FRAME_TYPE;
    afrm->htype = ARP_HTYPE;
    afrm->ptype = ARP_PTYPE;
    afrm->hlen = ARP_HLEN;
    afrm->plen = ARP_PLEN;
    afrm->oper = oper;
    mymemcpy(&afrm->src_mac_addr, local_macaddr, 6);
    mymemcpy(&afrm->src_ip_addr, local_ipaddr, 4);
    mymemset(&afrm->dst_mac_addr, 0, 6);
    mymemcpy(&afrm->dst_ip_addr, dst_ip_addr, 4);
}

void myip_make_ip_frame(IP_FRAME *ifrm, const uint8_t *dst_ip_addr, uint16_t hsz, uint16_t id, uint16_t proto)
{
    preamble2 *p = &ifrm->p;
    myip_update_ip_mac_addr(p, dst_ip_addr);

    p->type = IP_FRAME_TYPE;
    p->ver_ihl = IP_VER_IHL;
    p->dscp_ecn = 0x00;
    p->total_len = HTONS_16(hsz);
    p->id = HTONS_16(id);
    p->frag = HTONS_16(0x4000);
    p->ttl = IP_TTL;
    p->proto = proto;
    p->header_cksum = 0;
}

void myip_swap_addr(preamble3 *p)
{
    mymemcpy(p->dst, p->src, 6);
    mymemcpy(p->src, local_macaddr, 6);
    mymemcpy(p->dst_ip_addr, p->src_ip_addr, 4);
    mymemcpy(p->src_ip_addr, local_ipaddr, 4);

    uint16_t dst_port = p->dst_port;
    p->dst_port = p->src_port;
    p->src_port = dst_port;
}

void myip_udp_update_ip_mac_port(preamble3 *p, const uint8_t *dst_ip_addr, uint16_t dst_port, uint16_t src_port)
{
    myip_update_ip_mac_addr((preamble2*)p, dst_ip_addr);
    p->dst_port = HTONS_16(dst_port);
    p->src_port = HTONS_16(src_port);
}

void myip_arp_table_update(uint8_t *ip_addr, uint8_t *mac_addr)
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

uint16_t myip_eth_frm_handler(ETH_FRAME *frm, uint16_t sz)
{
    IP_FRAME* ipfrm = (IP_FRAME*)frm;
    if(sz)
    {
        if(frm->e.p.type == ARP_FRAME_TYPE)
            return myip_arp_frm_handler(frm, sz);

        if ((ipfrm->p.ver_ihl != IP_VER_IHL) || ((ipfrm->p.frag & FRAG_MASK) != 0))
            return 0;
        myip_arp_table_update(ipfrm->p.src_ip_addr, ipfrm->p.src);

#if 0
        if(ipfrm->p.proto == ICMP_PROTO)
            return myip_icmp_frm_handler(frm, sz);
#endif
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
            if(con_table[i].proto != ipfrm->p.proto)
                continue;
        }
        sz1 = con_table[i].frm_handler_ptr(frm, sz, i);
        if(sz1)
            break;
#if 0
        if(con_table[i].proto == ICMP_PROTO)
        {
            sz1 = myip_icmp_frm_handler(frm, sz, i);
            if(sz1)
                break;
        }
        if(con_table[i].proto == UDP_PROTO)
        {
            sz1 = myip_udp_frm_handler(frm, sz, i);
            if(sz1)
                break;
        }
        if(con_table[i].proto == TCP_PROTO)
        {
            sz1 = myip_tcp_frm_handler(frm, sz, i);
            if(sz1)
                break;
        }
#endif
    }
    return sz1;
}

uint16_t myip_arp_frm_handler(ETH_FRAME *frm, uint16_t sz)
{
    ARP_FRAME *afrm = (ARP_FRAME*)frm;
    if((afrm->htype != ARP_HTYPE) || (afrm->ptype != ARP_PTYPE) || \
      ((afrm->hlen) != ARP_HLEN) || ((afrm->plen) != ARP_PLEN)) {
        return 0;
    }
    if(mymemcmp(afrm->dst_ip_addr, local_ipaddr, 4))
        return 0;
    myip_arp_table_update(afrm->src_ip_addr, afrm->src_mac_addr);
    if(afrm->oper == ARP_OPER_REQ)
    {
        mymemcpy(afrm->dst_mac_addr, afrm->src_mac_addr, 6);
        mymemcpy(afrm->src_mac_addr, local_macaddr, 6);
        mymemcpy(afrm->dst_ip_addr, afrm->src_ip_addr, 4);
        mymemcpy(afrm->src_ip_addr, local_ipaddr, 4);
        afrm->oper = ARP_OPER_REPL;
        return sizeof(ARP_FRAME);
    }
    return 0;
}

uint16_t myip_udp_frm_handler(ETH_FRAME *frm, uint16_t sz, uint16_t con_index)
{
    UDP_FRAME* ufrm = (UDP_FRAME*)frm;
    if(con_index >= CON_TABLE_SZ)
        return 0;
    if(sz)
    {
        if(ufrm->p.proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].port != HTONS_16(ufrm->p.dst_port))
            return 0;
    }
    uint8_t *data_ptr;
    uint16_t data_sz = myip_udp_data(ufrm, sz, &data_ptr);
    //dbg_send_int2("myip_udp_con_handler", data_sz);
    data_sz = con_table[con_index].con_handler_ptr(data_ptr, data_sz);
    if(data_sz)
    {
        myip_udp_update_ip_mac_port(&ufrm->p, ufrm->p.src_ip_addr, HTONS_16(ufrm->p.src_port), HTONS_16(ufrm->p.dst_port));
        sz = UDPH_SZ + data_sz;
        ufrm->len = HTONS_16(sz);
        return MACH_SZ + IPH_SZ + sz;
    }
    return 0;
}

