#include "mytcp.h"

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
#define TCP_CTL 0x3f

static TCP_CONN tcp_conn;
extern uint8_t local_mac_addr[];
extern uint32_t local_ip_addr;
static uint8_t seqnum=0xA; // my initial tcp sequence number

void myip_tcp_init(void)
{
    tcp_conn.state = TCP_CON_CLOSED;
}

void myip_update_seqn_ackn(TCP_FRAME *tfrm)
{
    uint32_t tmp1, tmp2;
    tmp1 = HTONS_32(tfrm->seqn);
    tmp2 = HTONS_32(tfrm->ackn);
    tmp1 += 1;
    tfrm->ackn = HTONS_32(tmp1);
    //tmp2 += 1;
    tfrm->seqn = HTONS_32(tmp2);
}

uint16_t myip_handle_tcp_frame(ETH_FRAME *frm, uint16_t sz)
{
    TCP_FRAME *tfrm = (TCP_FRAME*)frm;
    uint32_t tmp1, tmp2;
    uint8_t *ptr;
    //io_send_hex4("pkt", &(tfrm->p.src_port), 16);
    //io_send_hex2("flags", tfrm->flags);
    switch(tfrm->flags & TCP_CTL & ~TCP_ACK)
    {
        case TCP_SYN:
            io_send_str3("TCP_SYN", 1);
            tfrm->flags |= TCP_ACK;
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
            tfrm->seqn = HTONS_32(seqnum);
            return sz;
        case TCP_RST:
            io_send_str3("TCP_RST", 1);
            break;
        case TCP_PSH:
            io_send_str3("TCP_PSH", 1);
            tmp1 = tfrm->offset >> 4;
            tmp2 = &tfrm->p.src_port;
            tmp2 += tmp1*4;
            ptr = tmp2;
            tmp1 = tfrm;
            tmp1 += sz;
            tmp1 -= tmp2;
            io_newline();
            io_send_str(ptr, tmp1);
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
            return sz;
        case TCP_FIN:
            io_send_str3("TCP_FIN", 1);
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
            return sz;
        default:
            break;
    }
    return 0;
}

