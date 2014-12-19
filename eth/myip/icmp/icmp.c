
#include "eth/eth.h"
#include "icmp/icmp.h"

extern uint8_t local_ipaddr[4];
static uint8_t ping_ipaddr[4] = {0, 0, 0, 0};

enum {PING_STATE_NONE, PING_STATE_START, PING_STATE_WAIT_ARP, PING_STATE_WAIT_REPLY, PING_STATE_OK};
static volatile uint16_t ping_state;

void myip_icmp_init(void)
{
    ping_state = PING_STATE_NONE;
}

uint16_t myip_icmp_con_handler(uint8_t *data, uint16_t sz)
{
    return 0;
}

void myip_make_icmp_frame(icmpfrm_t *ifrm, uint8_t *dst_ipaddr, uint8_t type, uint8_t code)
{
    myip_make_ip_frame((ipfrm_t*)ifrm, dst_ipaddr, ICMPH_SZ, ICMP_PROTO);
    ifrm->icmp.type = type;
    ifrm->icmp.code = code;
    ifrm->icmp.cksum = 0;
}

uint16_t myip_icmp_frm_handler(ethfrm_t *frm, uint16_t sz, uint16_t con_index)
{
    icmpfrm_t *ifrm = (icmpfrm_t*)frm;
    icmphdr_t *icmp = &ifrm->icmp;
    if(sz)
    {
        if(mymemcmp(ifrm->ip.dst_ip_addr, local_ipaddr, 4))
            return 0;
        uint8_t ipaddr[4];
        mymemcpy(ipaddr, ifrm->ip.src_ip_addr, 4);
        if(ifrm->icmp.type == ICMP_ECHO_REQUEST)
        {
            uint8_t ttl = ifrm->ip.ttl;
            myip_make_icmp_frame(ifrm, ipaddr, ICMP_ECHO_REPLY, 0);
            ifrm->ip.ttl = ttl;
            return MACH_SZ + IPH_SZ + ICMPH_SZ;
        }
        if(icmp->type == ICMP_ECHO_REPLY)
        {
            if(ping_state == PING_STATE_WAIT_REPLY)
            {
                if(!mymemcmp(ifrm->ip.src_ip_addr, ping_ipaddr, 4))
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
                myip_make_arp_frame((arpfrm_t*)frm, ping_ipaddr, ARP_OPER_REQ);
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

            myip_make_icmp_frame(ifrm, ping_ipaddr, ICMP_ECHO_REQUEST, 0);
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

