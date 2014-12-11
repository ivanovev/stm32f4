
#include "eth/eth.h"
#include "icmp/icmp.h"

extern ARP_ENTRY arp_table[ARP_TABLE_SZ];

extern uint8_t local_ipaddr[4];
extern uint8_t local_macaddr[6];
extern uint16_t ip_counter;
static uint8_t ping_ipaddr[4] = {0, 0, 0, 0};

enum {PING_STATE_NONE, PING_STATE_START, PING_STATE_WAIT_ARP, PING_STATE_WAIT_REPLY, PING_STATE_OK};
static volatile uint16_t ping_state;

void myip_icmp_init(void)
{
    ip_counter = 0;
    ping_state = PING_STATE_NONE;
}

uint16_t myip_icmp_con_handler(uint8_t *data, uint16_t sz)
{
    return 0;
}

uint16_t myip_make_icmp_frame(ICMP_FRAME *ifrm)
{
    return 0;
}

uint16_t myip_icmp_frm_handler(ETH_FRAME *frm, uint16_t sz, uint16_t con_index)
{
    if(sz)
    {
        ICMP_FRAME* ifrm = (ICMP_FRAME*)frm;
        if(mymemcmp(ifrm->p.dst_ip_addr, local_ipaddr, 4))
            return 0;
        uint8_t ipaddr[4];
        mymemcpy(ipaddr, ifrm->p.src_ip_addr, 4);
        if(ifrm->type == ICMP_ECHO_REQUEST)
        {
#if 0
            myip_update_ip_mac_addr(&ifrm->p, ifrm->p.src_ip_addr);
            ifrm->p.id = HTONS_16(ip_counter);
            ip_counter++;
            ifrm->p.ttl = IP_TTL;
            ifrm->p.header_cksum = 0;
            ifrm->cksum = 0;
            ifrm->type = ICMP_ECHO_REPLY;
            return sz;
#else
            myip_make_ip_frame((IP_FRAME*)frm, ipaddr, sz - MACH_SZ, ip_counter++, ICMP_PROTO);
            ifrm->type = ICMP_ECHO_REPLY;
            ifrm->code = 0;
            ifrm->cksum = 0;
            return sz;
#endif
        }
        if(ifrm->type == ICMP_ECHO_REPLY)
        {
            if(ping_state == PING_STATE_WAIT_REPLY)
            {
                if(!mymemcmp(ifrm->p.src_ip_addr, ping_ipaddr, 4))
                    ping_state = PING_STATE_OK;
            }
        }
    }
    else if(sz == 0)
    {
        if(ping_state == PING_STATE_NONE)
            return 0;
        if(ping_state == PING_STATE_OK)
            return 0;
        if(ping_state == PING_STATE_START)
        {
            uint16_t i = myip_arp_find(ping_ipaddr);
            if(i == ARP_TABLE_SZ)
            {
                myip_make_arp_frame((ARP_FRAME*)frm, ping_ipaddr, ARP_OPER_REQ);
                ping_state = PING_STATE_WAIT_ARP;
                return MACH_SZ + ARPH_SZ;
            }
            ping_state = PING_STATE_WAIT_ARP;
        }
        if(ping_state == PING_STATE_WAIT_ARP)
        {
            uint16_t i = myip_arp_find(ping_ipaddr);
            if(i == ARP_TABLE_SZ)
                return 0;

            ICMP_FRAME* ifrm = (ICMP_FRAME*)frm;

#if 1
            myip_make_ip_frame((IP_FRAME*)frm, ping_ipaddr, IPH_SZ + ICMPH_SZ, ip_counter++, ICMP_PROTO);
#else
            myip_update_ip_mac_addr(&ifrm->p, ping_ipaddr);
            ifrm->p.type = IP_FRAME_TYPE;
            ifrm->p.ver_ihl = IP_VER_IHL;
            ifrm->p.dscp_ecn = 0x00;
            ifrm->p.total_len = HTONS_16(IPH_SZ + ICMPH_SZ);
            ifrm->p.id = HTONS_16(ip_counter);
            ip_counter++;
            ifrm->p.frag = HTONS_16(0x4000);
            ifrm->p.ttl = IP_TTL;
            ifrm->p.proto = ICMP_PROTO;
            ifrm->p.header_cksum = 0;
#endif
            ifrm->type = ICMP_ECHO_REQUEST;
            ifrm->code = 0;
            ifrm->cksum = 0;
            ping_state = PING_STATE_WAIT_REPLY;
            return MACH_SZ + IPH_SZ + ICMPH_SZ;
        }
    }
    return 0;
}

uint16_t myip_icmp_ping(uint8_t *ipaddr)
{
    if(ipaddr)
    {
        mymemcpy(ping_ipaddr, ipaddr, 4);
        ping_state = PING_STATE_START;
    }
    if(ping_state == PING_STATE_OK)
        return 0;
    return 1;
}

