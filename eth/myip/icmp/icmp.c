
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

uint16_t myip_icmp_con_handler(uint8_t *in, uint16_t sz, uint8_t *out)
{
    return 0;
}

void myip_make_icmp_frame(icmpfrm_t *ifrm, uint8_t *dst_ipaddr, uint8_t type, uint8_t code, uint16_t id, uint16_t seq)
{
    myip_make_ip_frame((ipfrm_t*)ifrm, dst_ipaddr, ICMPH_SZ, ICMP_PROTO);
    ifrm->icmp.type = type;
    ifrm->icmp.code = code;
    ifrm->icmp.cksum = 0;
    ifrm->icmp.id = id;
    ifrm->icmp.seq = seq;
}

uint16_t myip_icmp_frm_handler(ethfrm_t *in, uint16_t sz, uint16_t con_index, ethfrm_t *out)
{
    icmpfrm_t *ifrmi = (icmpfrm_t*)in;
    icmpfrm_t *ifrmo = (icmpfrm_t*)out;
    if(sz)
    {
        if(mymemcmp(ifrmi->ip.dst_ip_addr, local_ipaddr, 4))
            return 0;
        uint8_t ipaddr[4];
        mymemcpy(ipaddr, ifrmi->ip.src_ip_addr, 4);
        if(ifrmi->icmp.type == ICMP_ECHO_REQUEST)
        {
            myip_make_icmp_frame(ifrmo, ipaddr, ICMP_ECHO_REPLY, 0, ifrmi->icmp.id, ifrmi->icmp.seq);
            ifrmo->ip.ttl = ifrmi->ip.ttl;
            return MACH_SZ + IPH_SZ + ICMPH_SZ;
        }
        if(ifrmi->icmp.type == ICMP_ECHO_REPLY)
        {
            if(ping_state == PING_STATE_WAIT_REPLY)
            {
                if(!mymemcmp(ifrmi->ip.src_ip_addr, ping_ipaddr, 4))
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
            myip_arp_request(ping_ipaddr);
            ping_state = PING_STATE_WAIT_ARP;
            return 0;
        }
        if(ping_state == PING_STATE_WAIT_ARP)
        {
            uint16_t i = myip_arp_find(ping_ipaddr);
            if(i == ARP_TABLE_SZ)
                return 0;

            myip_make_icmp_frame(ifrmo, ping_ipaddr, ICMP_ECHO_REQUEST, 0, 0, 0);
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

