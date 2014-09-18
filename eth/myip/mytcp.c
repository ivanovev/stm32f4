#include "mytcp.h"
#include "util/util.h"
#include "util/queue.h"

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
#define TCP_CTL 0x3f

extern uint8_t local_mac_addr[];
extern uint32_t local_ip_addr;
static uint8_t seqnum=0xA; // my initial tcp sequence number

//uint8_t telnetd_data1[] = {0xff, 0xfd, 0x18, 0xff, 0xfd, 0x20, 0xff, 0xfd, 0x23, 0xff, 0xfd, 0x27};
uint8_t telnetd_data1[] = "test";

#define TELNETD_SEND_DATA 1

static TCP_CONN tcp_conn;
Queue qtelnet;
uint8_t telnet_buf[IO_BUF_SZ];
static uint8_t telnetd_state = 0;

void myip_tcp_init(void)
{
    tcp_conn.state = TCP_CON_CLOSED;
    qtelnet.head = 0;
    qtelnet.tail = 0;
    telnetd_state = 0;
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

uint16_t myip_get_data(TCP_FRAME *tfrm, uint16_t sz, uint8_t **ptr)
{
    uint32_t tmp1, tmp2;
    tmp1 = tfrm->offset >> 4;
    tmp2 = &tfrm->p.src_port;
    tmp2 += tmp1*4;
    *ptr = tmp2;
    tmp1 = tfrm;
    tmp1 += sz;
    tmp1 -= tmp2;
    return (uint16_t)tmp1;
}

uint16_t myip_handle_telnetd(ETH_FRAME *frm, uint16_t sz)
{
    TCP_FRAME *tfrm = (TCP_FRAME*)frm;
    int tmp1, tmp2;
    int i;
    tmp1 = tfrm->offset >> 4;
    tmp1 -= 5;
    tmp1 *= 4;
    if(sz)
    {
        if(tfrm->data[tmp1] == 0xFF)
        {
            tfrm->flags = TCP_ACK;
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
#if 0
            tfrm->offset = 0x70;
            return MACIPTCPH_SZ + 8;
#else
            tmp1 = tfrm->offset >> 4;
            tmp1 *= 4;
            //for(i = 0; i < tmp1; i++)
            //tfrm->data[i] = 0;
            //tfrm->offset = 0x50;
            tfrm->p.header_cksum = 0;
            tfrm->cksum = 0;
            //telnetd_state = TELNETD_SEND_DATA;
            for(i = 0; i < sizeof(telnetd_data1); i++)
                enqueue(&qtelnet, telnetd_data1[i]);
            return MACH_SZ + IPH_SZ + tmp1;
#endif
        }
    }
    else
    {
        sz = dequeue(&qtelnet, (uint8_t*)telnet_buf, 0);
        if(!sz)
            return 0;
        io_send_int2("send telnetdata", sz);
        tfrm->flags = TCP_PSH | TCP_ACK;
        tfrm->ackn = 0;
        //tfrm->offset = 0x50;
        tmp1 = tfrm->offset >> 4;
        tmp1 -= 5;
        tmp1 *= 4;
        tfrm->p.type = IP_FRAME_TYPE;
        tfrm->p.header_cksum = 0;
        tfrm->cksum = 0;
        //for(i = 0; i < tmp1; i++)
            //tfrm->data[i] = 0;
        for(i = tmp1; i < (tmp1 + sz); i++)
            tfrm->data[i] = telnet_buf[i-tmp1];
        tmp1 = tfrm->offset >> 4;
        tmp1 *= 4;
        return MACH_SZ + IPH_SZ + tmp1 + sz;
    }
    return 0;
}

uint16_t myip_handle_tcp_frame(ETH_FRAME *frm, uint16_t sz)
{
    TCP_FRAME *tfrm = (TCP_FRAME*)frm;
    uint32_t tmp1, tmp2, i;
    uint8_t *ptr;
#if 0
    if(sz == 0)
        return myip_handle_telnetd(frm, sz);
#else
    if(!sz)
        return 0;
#endif
    //io_send_hex4("pkt", &(tfrm->p.src_port), 16);
    //io_send_int2("pkt_sz", sz);
    //io_send_int2("offset", tfrm->offset);
    //io_send_int2("ver_ihl", tfrm->p.ver_ihl);
#if 0
    if((tfrm->flags == TCP_ACK) && (tcp_conn.state = TCP_CON_LISTEN))
    {
        for(i = 0; i < sizeof(telnetd_data1); i++)
            enqueue(&qtelnet, telnetd_data1[i]);
        myip_swap_addr(&(tfrm->p));
        myip_update_seqn_ackn(tfrm);
        tcp_conn.state = TCP_CON_ESTABLISHED;
        return 0;
    }
#endif
    switch(tfrm->flags & TCP_CTL & ~TCP_ACK)
    {
        case TCP_SYN:
            io_send_str3("TCP_SYN", 1);
            tfrm->flags |= TCP_ACK;
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
            tfrm->seqn = HTONS_32(seqnum);
            tfrm->data[0] = 2;
            tfrm->data[1] = 4;
            tfrm->data[2] = 5;
            tfrm->data[3] = 0xB8;
            tfrm->offset = 0x60;
            tmp1 = tfrm->offset >> 4;
            tmp1 *= 4;
            tfrm->wndsz = 0xB805;
            sz = IPH_SZ + tmp1;
            tfrm->p.total_len = HTONS_16(sz);
            return MACH_SZ + sz;
        case TCP_RST:
            io_send_str3("TCP_RST", 1);
            break;
        case TCP_PSH:
            io_send_str3("TCP_PSH", 1);
            tmp1 = myip_get_data(tfrm, sz, &ptr);
            io_newline();
            io_send_str(ptr, tmp1);
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
            tmp1 = HTONS_32(tfrm->ackn);
            tmp1 -= 1;
            tfrm->ackn = HTONS_32(tmp1);
            tmp1 = tfrm->offset >> 4;
            tmp1 -= 5;
            tmp1 *= 4;
            for(i = tmp1; i < (tmp1 + 4); i++)
                tfrm->data[i] = telnetd_data1[i-tmp1];
            io_send_int2("tmp1", tmp1);
            tmp1 = tfrm->offset >> 4;
            tmp1 *= 4;
            sz = IPH_SZ + tmp1 + 4;
            tfrm->p.total_len = HTONS_16(sz);
            return MACH_SZ + sz;
#if 0
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
#else
            //sz = myip_handle_telnetd(frm, sz);
#endif
            //return sz;
        case TCP_FIN:
            io_send_str3("TCP_FIN", 1);
            myip_swap_addr(&(tfrm->p));
            myip_update_seqn_ackn(tfrm);
            tcp_conn.state = TCP_CON_CLOSED;
            telnetd_state = 0;
            return sz;
        default:
            break;
    }
    return 0;
}

