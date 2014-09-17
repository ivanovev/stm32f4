
#include "myip.h"
#include "mytcp.h"

ARP_ENTRY arp_table[ARP_TABLE_SZ];
UDP_ENTRY udp_con_table[UDP_TABLE_SZ];

uint32_t local_ip_addr = LOCAL_IP_ADDR;
uint8_t local_mac_addr[] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};
uint16_t ip_counter;

uint16_t dbg_con_handler(UDP_FRAME *ufrm, uint16_t sz);

void myip_init(void)
{
    uint32_t i;
    for(i = 0; i < ARP_TABLE_SZ; i++)
        arp_table[i].time = 0;
    for (i = 0; i < UDP_TABLE_SZ; i++)
    {
        udp_con_table[i].port = 0;
        udp_con_table[i].con_handler_ptr = 0;
    }
    myip_add_udp_con(1234, dbg_con_handler);
    ip_counter = 0;
    myip_tcp_init();
}

uint16_t dbg_con_handler(UDP_FRAME *ufrm, uint16_t sz)
{
    return sz;
}

void myip_add_udp_con(uint16_t port, con_handler con_handler_ptr)
{
    static uint8_t i = 0;
    udp_con_table[i].port = port;
    udp_con_table[i].con_handler_ptr = con_handler_ptr;
    i++;
}

void myip_update_arp_table(uint32_t ip_addr, uint8_t *mac_addr)
{
    uint32_t i;
    for(i = 0; i < ARP_TABLE_SZ; i++)
    {
        if(arp_table[i].ip_addr == ip_addr)
        {
            arp_table[i].ip_addr = ip_addr;
            mymemcpy(arp_table[i].mac_addr, mac_addr, 6);
            arp_table[i].time = 0;
            return;
        }
    }
    uint32_t max_time_index = 0;
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

uint16_t myip_handle_arp_frame(ETH_FRAME *frm, uint16_t sz)
{
    ARP_FRAME *afrm = (ARP_FRAME*)frm;
    if((afrm->htype != ARP_HTYPE) || (afrm->ptype != ARP_PTYPE) || \
      ((afrm->hlen) != ARP_HLEN) || ((afrm->plen) != ARP_PLEN)) {
        return 0;
    }
    if(HTONS_32(afrm->dst_ip_addr) != local_ip_addr)
        return 0;
    myip_update_arp_table(HTONS_32((uint32_t)afrm->src_ip_addr), afrm->src_mac_addr);
    switch (afrm->oper)
    {
        case ARP_OPER_REQ:
        {
            mymemcpy(afrm->dst_mac_addr, afrm->src_mac_addr, 6);
            mymemcpy(afrm->src_mac_addr, local_mac_addr, 6);

            afrm->oper = ARP_OPER_REPL;

            afrm->dst_ip_addr = afrm->src_ip_addr;
            afrm->src_ip_addr = HTONS_32(local_ip_addr);

            //sz = MAC_HEADER_SIZE + ARP_HEADER_SIZE;
            sz = sizeof(ARP_FRAME);
        }
    }
    return sz;
}

uint16_t myip_handle_ip_frame(ETH_FRAME *frm, uint16_t sz)
{
    IP_FRAME* ipfrm = (IP_FRAME*)frm;

    if ((ipfrm->p.ver_ihl != IP_VER_IHL) || ((ipfrm->p.frag_offset & FRAG_MASK) != 0))
        return 0;

    if (HTONS_32(ipfrm->p.dst_ip_addr) != local_ip_addr)
        return 0;

    myip_update_arp_table(HTONS_32((uint32_t)ipfrm->p.src_ip_addr), ipfrm->p.src);

#if 0
    uint16_t total_len = HTONS_16(ipfrm->p.total_len);
    if (total_len + MAC_HEADER_SIZE < sz)
        sz = total_len + MAC_HEADER_SIZE;
#endif

    switch (ipfrm->p.proto)
    {
        case ICMP_PROTO:
            sz = myip_handle_icmp_frame(frm, sz);
            break;

        case UDP_PROTO:
            sz = myip_handle_udp_frame(frm, sz);
            break;

        case TCP_PROTO:
            sz = myip_handle_tcp_frame(frm, sz);
            break;

        default:
            break;
    }
    return sz;
}

uint16_t myip_handle_icmp_frame(ETH_FRAME *frm, uint16_t sz)
{
    ICMP_FRAME* ifrm = (ICMP_FRAME*)frm;
    //io_send_str3("myip_handle_icmp_frame", 1);

    switch (ifrm->type) {
        case ICMP_ECHO:
            mymemcpy(ifrm->p.dst, ifrm->p.src, 6);
            mymemcpy(ifrm->p.src, local_mac_addr, 6);

            ifrm->p.dst_ip_addr = ifrm->p.src_ip_addr;
            ifrm->p.src_ip_addr = HTONS_32(local_ip_addr);

            ifrm->p.id = HTONS_16(ip_counter);
            ip_counter += 1;
            ifrm->p.ttl = IP_TTL;
            ifrm->p.header_cksum = 0;

            ifrm->type = ICMP_ECHO_REPLY;
            ifrm->cksum = 0;

            break;

        default:
            return 0;
    }
    return sz;
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

uint16_t myip_handle_udp_frame(ETH_FRAME *frm, uint16_t sz)
{
    UDP_FRAME* ufrm = (UDP_FRAME*)frm;
    io_send_str3("myip_handle_udp_frame", 1);

    uint8_t i = 0;
    io_send_hex2("dst_port", HTONS_16(ufrm->p.dst_port));
    for(i = 0; i < UDP_TABLE_SZ; i++)
    {
        if(udp_con_table[i].port == HTONS_16(ufrm->p.dst_port))
            break;
    }
    io_send_hex2("i", i);
    io_send_hex2("con_port", udp_con_table[i].port);

    if (i < UDP_TABLE_SZ)
    {
#if 0
        if(udp_con_table[i].con_handler_ptr)
            sz = udp_con_table[i].con_handler_ptr(ufrm, sz);
        else
            sz = 0;
#endif
        if(sz)
        {
#if 0
            mymemcpy(ufrm->p.dst, ufrm->p.src, 6);
            mymemcpy(ufrm->p.src, local_mac_addr, 6);

            ufrm->p.dst_ip_addr = ufrm->p.src_ip_addr;
            ufrm->p.src_ip_addr = HTONS_32(local_ip_addr);
            ufrm->dst_port = ufrm->src_port;
            ufrm->src_port = HTONS_16(udp_con_table[i].port);
#else
            myip_swap_addr(&(ufrm->p));
#endif
        }
    }
    return sz;
}

