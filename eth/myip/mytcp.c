
#ifdef ENABLE_TELNET

#include "mytcp.h"

#ifdef ENABLE_TELNET
#include "telnetd.h"
#endif

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

extern uint8_t local_ipaddr[4];
extern uint8_t local_macaddr[6];
extern CON_ENTRY con_table[CON_TABLE_SZ];

struct {
    volatile uint8_t state;
    uint8_t remote_mac_addr[6];
    uint8_t remote_ip_addr[4];
    uint16_t remote_port;
    uint16_t local_port;
    uint16_t id;

    uint32_t seqn;
    uint32_t ackn;
} tcp_con;

void myip_tcp_init(void)
{
    tcp_con.state = TCP_CON_CLOSED;
    tcp_con.seqn = 0;
    tcp_con.ackn = 0;
}

uint16_t myip_tcp_data(tcpfrm_t *tfrm, uint16_t sz, uint8_t **ptr)
{
    uint32_t tmp1, tmp2;
    tmp2 = (uint32_t)&tfrm->tcp.src_port;
    if(sz)
    {
        tmp1 = tfrm->tcp.offset >> 4;
        tmp2 += tmp1*4;
        *ptr = (uint8_t*)tmp2;
        return HTONS_16(tfrm->ip.total_len) - IPH_SZ - TCPH_SZ;
    }
    else
    {
        tmp2 += TCPH_SZ;
        *ptr = (uint8_t*)tmp2;
        //return ETH_MAX_PACKET_SIZE - IPH_SZ - TCPH_SZ;
        return 0;
    }
}

void myip_new_tcp_con(tcpfrm_t *tfrm)
{
    mymemcpy(tcp_con.remote_mac_addr, tfrm->mac.src, 6);
    mymemcpy(tcp_con.remote_ip_addr, tfrm->ip.src_ip_addr, 4);
    tcp_con.remote_port = HTONS_16(tfrm->tcp.src_port);
    tcp_con.local_port = HTONS_16(tfrm->tcp.dst_port);
    tcp_con.id = HTONS_16(tfrm->ip.id);
    tcp_con.seqn = 0xA; // my initial tcp sequence number
    tcp_con.ackn = HTONS_32(tfrm->tcp.seqn) + 1;
}

void myip_update_tcp_hdr(tcphdr_t *tcp)
{
    tcp->src_port = HTONS_16(tcp_con.local_port);
    tcp->dst_port = HTONS_16(tcp_con.remote_port);
    tcp->seqn = HTONS_32(tcp_con.seqn);
    tcp->ackn = HTONS_32(tcp_con.ackn);
    tcp->offset = (TCPH_SZ / 4) << 4;
    tcp->wndsz = HTONS_16(TCP_MSS);
    tcp->cksum = 0;
    tcp->urg = 0;
}

void myip_make_tcp_frame(tcpfrm_t *tfrm, uint16_t data_sz)
{
    myip_make_ip_frame((ipfrm_t*)tfrm, tcp_con.remote_ip_addr, TCPH_SZ + data_sz, TCP_PROTO);
    myip_update_tcp_hdr(&tfrm->tcp);
}

uint16_t myip_tcp_frm_handler(ethfrm_t *frm, uint16_t sz, uint16_t con_index)
{
    if(con_index >= CON_TABLE_SZ)
        return 0;
    tcpfrm_t *tfrm = (tcpfrm_t*)frm;
    tcphdr_t *tcp = &tfrm->tcp;
    uint8_t flags = 0;
    uint8_t *ptr;
    uint16_t sz1 = 0, sz2 = 0;
    uint32_t seqn = HTONS_32(tcp->seqn);
    uint32_t ackn = HTONS_32(tcp->ackn);
    if(sz > 0)
    {
        if(tfrm->ip.proto != TCP_PROTO)
            return 0;
        if(con_table[con_index].proto != TCP_PROTO)
            return 0;
        if(con_table[con_index].port != HTONS_16(tcp->dst_port))
            return 0;
        if(tcp_con.state != TCP_CON_CLOSED)
        {
            if(mymemcmp(tcp_con.remote_ip_addr, tfrm->ip.src_ip_addr, 4))
                return 0;
            if(tcp_con.remote_port != HTONS_16(tcp->src_port))
                return 0;
        }
        flags = tcp->flags & TCP_CTL & ~TCP_ACK;
    }
    if(sz == 0)
    {
        if(myip_tcp_con_closed())
            return 0;
    }
    if(flags == TCP_SYN)
    {
        if(tcp_con.state != TCP_CON_CLOSED)
            return 0;
        tcp_con.state = TCP_CON_LISTEN;
        myip_new_tcp_con(tfrm);
        myip_make_tcp_frame(tfrm, TCP_OPT_MSS_LEN);
#ifdef ENABLE_TELNET
        myip_telnetd_init();
#endif
        tcp_con.seqn += 1;
        tcp->flags |= TCP_ACK;
        tfrm->data[0] = TCP_OPT_MSS;
        tfrm->data[1] = TCP_OPT_MSS_LEN;
        tfrm->data[2] = TCP_MSS / 256;
        tfrm->data[3] = TCP_MSS & 255;
        tcp->offset = ((TCPH_SZ + TCP_OPT_MSS_LEN) / 4) << 4;

        return MACH_SZ + IPH_SZ + TCPH_SZ + TCP_OPT_MSS_LEN;
    }
    if(tcp_con.state == TCP_CON_CLOSED)
        return 0;
    if(con_table[con_index].port != tcp_con.local_port)
        return 0;
    if(flags == TCP_FIN)
    {
        myip_new_tcp_con(tfrm);
        tcp_con.ackn = HTONS_32(tcp->seqn) + 1;
        tcp_con.seqn = HTONS_32(tcp->ackn);
        myip_make_tcp_frame(tfrm, 0);
        if(tcp_con.state == TCP_CON_CLOSE)
            tcp->flags = TCP_ACK;
        else
            tcp->flags = TCP_FIN | TCP_ACK;
        tcp_con.state = TCP_CON_CLOSED;
        myip_tcp_init();
        return sz;
    }
    if(tcp_con.state == TCP_CON_CLOSE)
    {
        tcp->flags = TCP_FIN | TCP_ACK;
        myip_make_tcp_frame(tfrm, 0);
        tcp_con.state = TCP_CON_CLOSED;
        return MACH_SZ + IPH_SZ + TCPH_SZ;
    }
    sz1 = myip_tcp_data(tfrm, sz, &ptr);
    if((sz > 0) && (sz1 == 0) && (tcp->flags == TCP_ACK))
        return 0;
    if((sz > 0) && (sz1 > 0) && ((seqn + sz1) < tcp_con.ackn))
    {
        dbg_send_hex2("seqn", seqn);
        dbg_send_hex2("sz1", sz1);
        dbg_send_hex2("ackn", ackn);

        myip_make_tcp_frame(tfrm, sz1);
        tcp->ackn = HTONS_32((seqn + sz1));
        //tfrm->seqn = HTONS_32(ackn);
        tcp->flags = TCP_ACK;
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
            tcp->flags = TCP_FIN | TCP_ACK;
            dbg_send_str3("tcp_fin", 1);
            tcp_con.ackn += 1;
        }
        else if(sz2 > 0)
            tcp->flags = TCP_PSH | TCP_ACK;
        else
            tcp->flags = TCP_ACK;
        tcp_con.id++;
        myip_make_tcp_frame(tfrm, sz2);
        tcp_con.seqn += sz2;
        return MACH_SZ + IPH_SZ + TCPH_SZ + sz2;
    }
    return 0;
}

void myip_tcp_con_close(void)
{
    tcp_con.state = TCP_CON_CLOSE;
}

uint16_t myip_tcp_con_closed(void)
{
    return tcp_con.state == TCP_CON_CLOSED;
}

#endif
