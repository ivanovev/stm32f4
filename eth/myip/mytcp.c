
#include "mytcp.h"
#include "mytelnetd.h"
#include "uart/uart.h"

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20
#define TCP_CTL 0x3f

#define TCP_OPT_END     0
#define TCP_OPT_NOOP    1
#define TCP_OPT_MSS     2

#define TCP_OPT_MSS_LEN 4

#define TCP_MSS ETH_MAX_PACKET_SIZE
//#define TCP_MSS (MACH_SZ + IPH_SZ + TCPH_SZ + 512)

extern uint8_t local_mac_addr[];
extern uint32_t local_ip_addr;
extern CON_ENTRY con_table[CON_TABLE_SZ];

TCP_CON tcp_con;
static uint8_t seqnum = 0xA; // my initial tcp sequence number

void myip_tcp_init(void)
{
    tcp_con.state = TCP_CON_CLOSED;
    tcp_con.seqn = 0;
    tcp_con.ackn = 0;
    tcp_con.rxseqn0 = 0;
    myip_telnetd_init();
}

uint16_t myip_tcp_data(TCP_FRAME *tfrm, uint16_t sz, uint8_t **ptr)
{
    uint32_t tmp1, tmp2;
    tmp2 = (uint32_t)&tfrm->p.src_port;
    if(sz)
    {
        tmp1 = tfrm->offset >> 4;
        tmp2 += tmp1*4;
        *ptr = (uint8_t*)tmp2;
        return HTONS_16(tfrm->p.total_len) - IPH_SZ - TCPH_SZ;
    }
    else
    {
        tmp2 += TCPH_SZ;
        *ptr = (uint8_t*)tmp2;
        //return ETH_MAX_PACKET_SIZE - IPH_SZ - TCPH_SZ;
        return 0;
    }
}

void myip_get_addr(TCP_FRAME *tfrm, TCP_CON *con)
{
    mymemcpy(tcp_con.remote_mac_addr, tfrm->p.src, 6);
    tcp_con.remote_ip_addr = HTONS_32(tfrm->p.src_ip_addr);
    tcp_con.remote_port = HTONS_16(tfrm->p.src_port);
    tcp_con.local_port = HTONS_16(tfrm->p.dst_port);
    tcp_con.id = HTONS_16(tfrm->p.id);
    tcp_con.seqn = seqnum;
    tcp_con.ackn = HTONS_32(tfrm->seqn) + 1;
}

void myip_update_frame(TCP_FRAME *tfrm, TCP_CON *con)
{
    mymemcpy(tfrm->p.dst, tcp_con.remote_mac_addr, 6);
    mymemcpy(tfrm->p.src, local_mac_addr, 6);
    tfrm->p.type = IP_FRAME_TYPE;

    tfrm->p.ver_ihl = IP_VER_IHL;
    tfrm->p.dscp_ecn = 0x00;
    tfrm->p.total_len = HTONS_16(IPH_SZ + TCPH_SZ);
    tfrm->p.id = HTONS_16(tcp_con.id);
    tfrm->p.frag = HTONS_16(0x4000);
    tfrm->p.ttl = IP_TTL;
    tfrm->p.proto = TCP_PROTO;
    tfrm->p.header_cksum = 0;
    tfrm->p.src_ip_addr = HTONS_32(local_ip_addr);
    tfrm->p.dst_ip_addr = HTONS_32(tcp_con.remote_ip_addr);

    tfrm->p.src_port = HTONS_16(tcp_con.local_port);
    tfrm->p.dst_port = HTONS_16(tcp_con.remote_port);
    tfrm->seqn = HTONS_32(tcp_con.seqn);
    tfrm->ackn = HTONS_32(tcp_con.ackn);
    tfrm->offset = (TCPH_SZ / 4) << 4;
    tfrm->wndsz = HTONS_16(TCP_MSS);
    tfrm->cksum = 0;
    tfrm->urg = 0;
}

uint16_t myip_tcp_con_handler(ETH_FRAME *frm, uint16_t sz, uint8_t con_index)
{
    TCP_FRAME *tfrm = (TCP_FRAME*)frm;
    //uart_send_int2("myip_tcp_con_handler", sz);
    if (con_index >= CON_TABLE_SZ)
        return 0;
    uint8_t flags = 0;
    uint8_t *ptr;
    uint16_t sz1 = 0, sz2 = 0;
    uint32_t seqn = HTONS_32(tfrm->seqn);
    uint32_t ackn = HTONS_32(tfrm->ackn);
    if(sz > 0)
    {
        if(tfrm->p.proto != TCP_PROTO)
            return 0;
        if(con_table[con_index].proto != TCP_PROTO)
            return 0;
        if(con_table[con_index].port != HTONS_16(tfrm->p.dst_port))
            return 0;
        if(tcp_con.state != TCP_CON_CLOSED)
        {
            if(tcp_con.remote_ip_addr != HTONS_32(tfrm->p.src_ip_addr))
                return 0;
            if(tcp_con.remote_port != HTONS_16(tfrm->p.src_port))
                return 0;
        }
        flags = tfrm->flags & TCP_CTL & ~TCP_ACK;
    }
    if(flags == TCP_SYN)
    {
        if(tcp_con.state != TCP_CON_CLOSED)
            return 0;
        tcp_con.rxseqn0 = HTONS_32(tfrm->seqn);
        tcp_con.state = TCP_CON_LISTEN;
        myip_get_addr(tfrm, &tcp_con);
        myip_update_frame(tfrm, &tcp_con);
        myip_telnetd_init();

        tcp_con.seqn += 1;
        tfrm->flags |= TCP_ACK;
        tfrm->data[0] = TCP_OPT_MSS;
        tfrm->data[1] = TCP_OPT_MSS_LEN;
        tfrm->data[2] = TCP_MSS / 256;
        tfrm->data[3] = TCP_MSS & 255;
        tfrm->offset = ((TCPH_SZ + TCP_OPT_MSS_LEN) / 4) << 4;
        tfrm->p.total_len = HTONS_16(IPH_SZ + TCPH_SZ + TCP_OPT_MSS_LEN);

        return MACH_SZ + IPH_SZ + TCPH_SZ + TCP_OPT_MSS_LEN;
    }
    if(flags == TCP_FIN)
    {
        myip_get_addr(tfrm, &tcp_con);
        tcp_con.ackn = HTONS_32(tfrm->seqn) + 1;
        tcp_con.seqn = HTONS_32(tfrm->ackn);
        myip_update_frame(tfrm, &tcp_con);
        if(tcp_con.state == TCP_CON_CLOSE)
            tfrm->flags = TCP_ACK;
        else
            tfrm->flags = TCP_FIN | TCP_ACK;
        tcp_con.state = TCP_CON_CLOSED;
        myip_tcp_init();
        return sz;
    }
    if(tcp_con.state == TCP_CON_CLOSE)
    {
        tfrm->flags = TCP_FIN | TCP_ACK;
        myip_update_frame(tfrm, &tcp_con);
        tfrm->p.total_len = HTONS_16(IPH_SZ + TCPH_SZ);
        return MACH_SZ + IPH_SZ + TCPH_SZ;
    }
#if 0
    if(flags & TCP_FIN)
    {
        tcp_con.state = TCP_CON_CLOSE;
        tcp_con.ackn += 1;
    }
#endif
    //if(sz)
        //uart_send_int2("myip_tcp_con_handler.psh_sz", sz);
    if(tcp_con.state == TCP_CON_CLOSED)
        return 0;
    if(con_table[con_index].port != tcp_con.local_port)
        return 0;
    sz1 = myip_tcp_data(tfrm, sz, &ptr);
    if((sz > 0) && (sz1 == 0) && (tfrm->flags == TCP_ACK))
        return 0;
    if((sz > 0) && (sz1 > 0) && ((seqn + sz1) < tcp_con.ackn))
    {
#ifdef MY_UART
        uart_send_hex2("seqn", seqn);
        uart_send_hex2("sz1", sz1);
        uart_send_hex2("ackn", ackn);
#endif
        myip_update_frame(tfrm, &tcp_con);
        tfrm->ackn = HTONS_32((seqn + sz1));
        //tfrm->seqn = HTONS_32(ackn);
        tfrm->flags = TCP_ACK;
        return MACH_SZ + IPH_SZ + TCPH_SZ;
    }
    sz2 = con_table[con_index].con_handler_ptr(ptr, sz ? sz1 : 0);
    if(sz || sz2)
    {
        if(sz)
            tcp_con.ackn += sz1;
        if(flags & TCP_FIN)
        {
            tcp_con.state = TCP_CON_CLOSED;
            tfrm->flags = TCP_FIN | TCP_ACK;
            tcp_con.ackn += 1;
        }
        else if(sz2 > 0)
            tfrm->flags = TCP_PSH | TCP_ACK;
        else
            tfrm->flags = TCP_ACK;
        tcp_con.id++;
        myip_update_frame(tfrm, &tcp_con);
        tcp_con.seqn += sz2;
        tfrm->p.total_len = HTONS_16(IPH_SZ + TCPH_SZ + sz2);
        return MACH_SZ + IPH_SZ + TCPH_SZ + sz2;
    }
    return 0;
}

uint16_t myip_tcp_con_closed(void)
{
    return tcp_con.state == TCP_CON_CLOSED;
}

