
#include "myip.h"
#include "mytcp.h"
#include "mytelnetd.h"
#include "mydatad.h"
#include "myptpd.h"

ARP_ENTRY arp_table[ARP_TABLE_SZ];
CON_ENTRY con_table[CON_TABLE_SZ];

uint32_t local_ip_addr = LOCAL_IP_ADDR;
uint8_t local_mac_addr[] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};
uint16_t ip_counter;

uint16_t dbg_con_handler(uint8_t *data, uint16_t sz);

void myip_init(void)
{
    uint32_t i;
    for(i = 0; i < ARP_TABLE_SZ; i++)
        arp_table[i].time = 0;
    for (i = 0; i < CON_TABLE_SZ; i++)
    {
        con_table[i].port = 0;
        con_table[i].state = 0;
        con_table[i].con_handler_ptr = 0;
    }
    myip_con_add(UDP_PORT_DBG, UDP_PROTO, dbg_con_handler);
    myip_con_add(TCP_PORT_TELNET, TCP_PROTO, myip_telnetd_io);
    myip_con_add(TCP_PORT_DATA, TCP_PROTO, myip_datad_io);
    myip_con_add(UDP_PORT_PTP, UDP_PROTO, myip_ptpd_io);
    ip_counter = 0;
    myip_tcp_init();
#ifdef MY_I2C
    local_ip_addr = eeprom_ipaddr_read();
#endif
}

uint16_t dbg_con_handler(uint8_t *data, uint16_t sz)
{
    return 0;
}

void myip_con_add(uint16_t port, uint8_t proto, con_handler con_handler_ptr)
{
    static uint8_t i = 0;
    con_table[i].port = port;
    con_table[i].proto = proto;
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
    else
        return ETH_MAX_PACKET_SIZE - IPH_SZ - UDPH_SZ;
}

uint16_t myip_arp_find(uint32_t ip_addr)
{
    uint16_t i;
    for(i = 0; i < ARP_TABLE_SZ; i++)
    {
        if(arp_table[i].ip_addr == ip_addr)
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

void myip_swap_addr(preamble3 *p)
{
    mymemcpy(p->dst, p->src, 6);
    mymemcpy(p->src, local_mac_addr, 6);

    p->dst_ip_addr = p->src_ip_addr;
    p->src_ip_addr = HTONS_32(local_ip_addr);
    uint16_t dst_port = p->dst_port;
    p->dst_port = p->src_port;
    p->src_port = dst_port;
}

void myip_update_ip_mac_addr(preamble2 *p, uint32_t dst_ip_addr)
{
    uint16_t i = myip_arp_find(dst_ip_addr);
    mymemcpy(p->dst, arp_table[i].mac_addr, 6);
    mymemcpy(p->src, local_mac_addr, 6);
    p->dst_ip_addr = HTONS_32(dst_ip_addr);
    p->src_ip_addr = HTONS_32(local_ip_addr);
}

void myip_update_ip_mac_addr_and_port(preamble3 *p, uint32_t dst_ip_addr, uint16_t dst_port, uint16_t src_port)
{
    myip_update_ip_mac_addr((preamble2*)p, dst_ip_addr);
    p->dst_port = dst_port;
    p->src_port = src_port;
}

void myip_arp_table_update(uint32_t ip_addr, uint8_t *mac_addr)
{
    uint16_t i = myip_arp_find(ip_addr);
    if(i < ARP_TABLE_SZ)
    {
        arp_table[i].ip_addr = ip_addr;
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
    arp_table[max_time_index].ip_addr = ip_addr;
    mymemcpy(arp_table[max_time_index].mac_addr, mac_addr, 6);
    arp_table[max_time_index].time = 0;
}

uint16_t myip_eth_frame_handler(ETH_FRAME *frm, uint16_t sz)
{
    IP_FRAME* ipfrm = (IP_FRAME*)frm;
    if(sz)
    {
        if(frm->e.p.type == ARP_FRAME_TYPE)
            return myip_arp_frame_handler(frm, sz);

        if ((ipfrm->p.ver_ihl != IP_VER_IHL) || ((ipfrm->p.frag & FRAG_MASK) != 0))
            return 0;
        if (HTONS_32(ipfrm->p.dst_ip_addr) != local_ip_addr)
            return 0;
        myip_arp_table_update(HTONS_32((uint32_t)ipfrm->p.src_ip_addr), ipfrm->p.src);

        if(ipfrm->p.proto == ICMP_PROTO)
            return myip_icmp_frame_handler(frm, sz);
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
        //uart_send_int2("myip_eth_frame_handler.i", i);
        if(!con_table[i].con_handler_ptr)
            break;
        if(con_table[i].proto == UDP_PROTO)
        {
            sz1 = myip_udp_con_handler(frm, sz, i);
            if(sz1)
                break;
        }
        if(con_table[i].proto == TCP_PROTO)
        {
            sz1 = myip_tcp_con_handler(frm, sz, i);
            if(sz1)
                break;
        }
    }
    return sz1;
}

uint16_t myip_arp_frame_handler(ETH_FRAME *frm, uint16_t sz)
{
    ARP_FRAME *afrm = (ARP_FRAME*)frm;
    if((afrm->htype != ARP_HTYPE) || (afrm->ptype != ARP_PTYPE) || \
      ((afrm->hlen) != ARP_HLEN) || ((afrm->plen) != ARP_PLEN)) {
        return 0;
    }
    if(HTONS_32(afrm->dst_ip_addr) != local_ip_addr)
        return 0;
    myip_arp_table_update(HTONS_32((uint32_t)afrm->src_ip_addr), afrm->src_mac_addr);
    if(afrm->oper == ARP_OPER_REQ)
    {
        mymemcpy(afrm->dst_mac_addr, afrm->src_mac_addr, 6);
        mymemcpy(afrm->src_mac_addr, local_mac_addr, 6);
        afrm->dst_ip_addr = afrm->src_ip_addr;
        afrm->src_ip_addr = HTONS_32(local_ip_addr);
        afrm->oper = ARP_OPER_REPL;
        return sizeof(ARP_FRAME);
    }
    return 0;
}

uint16_t myip_icmp_frame_handler(ETH_FRAME *frm, uint16_t sz)
{
    ICMP_FRAME* ifrm = (ICMP_FRAME*)frm;
    if(ifrm->type == ICMP_ECHO)
    {
        myip_update_ip_mac_addr(&ifrm->p, HTONS_32(ifrm->p.src_ip_addr));
        ifrm->p.id = HTONS_16(ip_counter);
        ip_counter += 1;
        ifrm->p.ttl = IP_TTL;
        ifrm->p.header_cksum = 0;
        ifrm->cksum = 0;
        ifrm->type = ICMP_ECHO_REPLY;
        return sz;
    }
    return 0;
}

uint16_t myip_udp_con_handler(ETH_FRAME *frm, uint16_t sz, uint8_t con_index)
{
    UDP_FRAME* ufrm = (UDP_FRAME*)frm;
    if (con_index >= CON_TABLE_SZ)
        return 0;
    if(sz)
    {
        if(ufrm->p.proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].port != ufrm->p.dst_port)
            return 0;
    }
    uint8_t *data_ptr;
    uint16_t data_sz = myip_udp_data(ufrm, sz, &data_ptr);
    data_sz = con_table[con_index].con_handler_ptr(data_ptr, data_sz);
    if(data_sz)
    {
        myip_update_ip_mac_addr_and_port(&ufrm->p, HTONS_32(ufrm->p.src_ip_addr), HTONS_16(ufrm->p.src_port), HTONS_16(ufrm->p.dst_port));
        return MACH_SZ + UDPH_SZ + data_sz;
    }
    return 0;
}

#if 0
uint16_t myip_udp_frame_handler(ETH_FRAME *frm, uint16_t sz)
{
    UDP_FRAME* ufrm = (UDP_FRAME*)frm;
    uint16_t i = myip_con_find(HTONS_16(ufrm->p.dst_port), UDP_PROTO);
    if (i >= CON_TABLE_SZ)
        return 0;
    sz = con_table[i].con_handler_ptr(frm, sz);
    if(sz)
        myip_update_ip_mac_addr_and_port(&ufrm->p, HTONS_32(ufrm->p.src_ip_addr), HTONS_16(ufrm->p.src_port), HTONS_16(ufrm->p.dst_port));
    return sz;
}
#endif

