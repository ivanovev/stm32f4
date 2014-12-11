
#include "eth/eth.h"
#include "ptp/myptpd.h"
#include "util/system_msp.h"

#define PTP_SYNC        0
#define PTP_DELAY_REQ   1
#define PTP_PDELAY_REQ  2
#define PTP_PDELAY_RESP 3
#define PTP_FOLLOW_UP   8
#define PTP_DELAY_RESP  9
#define PTP_PDELAY_RESP_FOLLOW_UP   0xA

#define PTP_LEN_SYNC        44
#define PTP_LEN_FOLLOW_UP   44
#define PTP_LEN_DELAY_REQ   44
#define PTP_LEN_DELAY_RESP  54
#define PTP_LEN_PDELAY_REQ  54
#define PTP_LEN_PDELAY_RESP  54
#define PTP_LEN_PDELAY_RESP_FOLLOW_UP  54

extern CON_ENTRY con_table[CON_TABLE_SZ];

struct ptpclk_t
{
    ptpts_t t1, t2, t3, t4;
    ptpts_t t_cmp;
    uint8_t clock_id[8];
    uint16_t source_port_id;
    uint16_t sequence_id;
    uint8_t log_msg;
    uint8_t state, wait;
    uint16_t next_msg_id;
    ptpts_t* save_ts;
    uint32_t sync_ts;
    ptpdt_t delay, offset;
    ptpmsg_delay_resp_t req;
} pc;

extern uint8_t local_ipaddr[4];
extern uint8_t local_macaddr[6];
extern uint16_t ip_counter;
const uint8_t mcast_ipaddr[4] = {224, 0, 0, 107};
const uint8_t mcast_macaddr[6] = {0x01, 0x1B, 0x19, 0x00, 0x00, 0x00};
const uint8_t p2p_ipaddr[4] = {224, 0, 0, 107};

static void ptp_calculate_delay_and_offset(void);

static void myip_ptpd_init_clock_id(void)
{
    mymemcpy(&pc.clock_id[0], &local_macaddr[0], 3);
    pc.clock_id[3] = 0xFF;
    pc.clock_id[4] = 0xFE;
    mymemcpy(&pc.clock_id[5], &local_macaddr[3], 3);
    pc.source_port_id = (uint16_t)pc.clock_id[7];
}

void myip_ptpd_init(void)
{
    myip_ptpd_init_clock_id();
#ifdef ENABLE_I2C
    eeprom_ptp_port_id_read(&pc.source_port_id);
    if(pc.source_port_id == 0xFFFF)
        pc.source_port_id = (uint16_t)pc.clock_id[7];
#endif
    pc.state = PTP_OFF;
    pc.wait = 0;
    pc.log_msg = 0x7F;
    pc.save_ts = 0;
    pc.sync_ts = 0;
}

uint16_t myip_ptpd_get_state(void)
{
    return pc.state;
}

void myip_ptpd_set_state(uint16_t state)
{
    pc.state = state;
    if(pc.state & PTP_E2E)
        pc.next_msg_id = PTP_SYNC;
    if(pc.state & PTP_MASTER)
    {
        myip_ptpd_init_clock_id();
        pc.sequence_id = 0;
        pc.sync_ts = uptime();
        pc.log_msg = 0;
    }
    else
    {
        pc.log_msg = 0x7F;
    }
    if(pc.state == PTP_P2P_SLAVE)
        pc.next_msg_id = PTP_PDELAY_REQ;
    else
        pc.next_msg_id = PTP_SYNC;
}

static void prepare_ptp_msg(ptpmsg_t *msg, uint8_t msg_id, uint16_t msg_len, uint8_t ctrl, ptpts_t *pts)
{
    mymemset((char*)msg, 0, msg_len);
    msg->msg_id = 0x80 | msg_id;
    msg->ptp_version = (uint8_t)0x02;
    msg->msg_len = HTONS_16(msg_len);
    msg->correction[0] = 0;
    msg->correction[1] = 0;
    mymemcpy(msg->clock_id, pc.clock_id, 8);
    msg->source_port_id = HTONS_16(pc.source_port_id);
    msg->sequence_id = HTONS_16(pc.sequence_id);
    msg->control_field = ctrl;
    msg->log_msg_interval = pc.log_msg;
    msg->s0 = 0;
    if(pts == 0)
    {
        ptpts_t ts;
        eth_ptpclk_time(&ts);
        pts = &ts;
    }
    msg->s = HTONS_32(pts->s);
    msg->ns = HTONS_32(pts->ns);
    if(msg_id == PTP_DELAY_RESP)
    {
        uint8_t *ptr = (uint8_t*)msg;
        ptpmsg_delay_resp_t *resp = (ptpmsg_delay_resp_t*)(ptr + sizeof(ptpmsg_t));
        mymemcpy(resp->clock_id, pc.req.clock_id, 8);
        resp->source_port_id = HTONS_16(pc.req.source_port_id);
    }
}

void myip_ptpd_save_ts(ptpts_t *t)
{
    if(pc.save_ts == 0)
    {
        return;
    }
    pc.save_ts->s = t->s;
    pc.save_ts->ns = t->ns;
    pc.save_ts = 0;
    return;
}

static uint16_t handle_pdelay_req(ptpmsg_t *msg, uint16_t sz)
{
    uint8_t *ptr = (uint8_t*)msg;
    ptpmsg_delay_resp_t *resp = (ptpmsg_delay_resp_t*)(ptr + sizeof(ptpmsg_t));

    mymemcpy((uint8_t*)pc.req.clock_id, (uint8_t*)msg->clock_id, 8);
    pc.req.source_port_id = msg->source_port_id;
    mymemcpy(resp->clock_id, pc.req.clock_id, 8);
    resp->source_port_id = pc.req.source_port_id;

    pc.sequence_id = HTONS_16(msg->sequence_id);
    prepare_ptp_msg(msg, PTP_PDELAY_RESP, PTP_LEN_PDELAY_RESP, CTRL_OTHER, &pc.t2);
    return PTP_LEN_PDELAY_RESP;
}

static uint16_t handle_pdelay_resp(ptpmsg_t *msg, uint16_t sz)
{
    eth_ptpts_get(&pc.t4, 0);
    pc.t2.s = HTONS_32(msg->s);
    pc.t2.ns = HTONS_32(msg->ns);
    //pc.sequence_id = HTONS_16(msg->sequence_id);
    return 0;
}

static uint16_t handle_pdelay_resp_follow_up(ptpmsg_t *msg, uint16_t sz)
{
    pc.t3.s = HTONS_32(msg->s);
    pc.t3.ns = HTONS_32(msg->ns);
    pc.state = PTP_OFF;
    dbg_send_int2("t1.s", pc.t1.s);
    dbg_send_int2("t1.ns", pc.t1.ns);
    dbg_send_int2("t2.s", pc.t2.s);
    dbg_send_int2("t2.ns", pc.t2.ns);
    dbg_send_int2("t3.s", pc.t3.s);
    dbg_send_int2("t3.ns", pc.t3.ns);
    dbg_send_int2("t4.s", pc.t4.s);
    dbg_send_int2("t4.ns", pc.t4.ns);
    ptpts_div2(&pc.t1);
    ptpts_div2(&pc.t2);
    ptpts_div2(&pc.t3);
    ptpts_div2(&pc.t4);
    ptpts_t sum1, sum2;
    ptpts_sum(&pc.t2, &pc.t4, &sum1);
    ptpts_sum(&pc.t1, &pc.t3, &sum2);
    ptpts_diff(&sum1, &sum2, &pc.delay);
    dbg_send_int2("delay.s", pc.delay.s);
    dbg_send_int2("delay.ns", pc.delay.ns);
    return 0;
}

static uint16_t ptpmsg_handle(uint8_t *data, uint16_t sz)
{
    if(!sz)
        return 0;
    ptpmsg_t *msg = (ptpmsg_t*)data;
    uint8_t msg_id = msg->msg_id & 0x0F;
    if(pc.state == PTP_E2E_MASTER)
    {
        if(msg_id == PTP_DELAY_REQ)
        {
            if(pc.sequence_id != HTONS_16(msg->sequence_id))
                return 0;

            mymemcpy(pc.req.clock_id, msg->clock_id, 8);
            pc.req.source_port_id = HTONS_16(msg->source_port_id);
            dbg_send_hex2("source_port_id", pc.req.source_port_id);
            eth_ptpts_get(&pc.t4, 0);
            prepare_ptp_msg(msg, PTP_DELAY_RESP, PTP_LEN_DELAY_RESP, CTRL_OTHER, &pc.t4);
            return PTP_LEN_DELAY_RESP;
        }
    }
    if(pc.state == PTP_E2E_SLAVE)
    {
        if(msg_id == PTP_SYNC)
        {
            eth_ptpts_get(&pc.t2, 0);
            pc.sequence_id = HTONS_16(msg->sequence_id);
            pc.next_msg_id = PTP_FOLLOW_UP;
            mymemcpy((char*)pc.req.clock_id, (char*)msg->clock_id, 8);
            pc.req.source_port_id = HTONS_16(msg->source_port_id);
            return 0;
        }
        if(msg_id != pc.next_msg_id)
            return 0;
        if(pc.req.source_port_id != HTONS_16(msg->source_port_id))
            return 0;
        if(mymemcmp((char*)pc.req.clock_id, (char*)msg->clock_id, 8))
            return 0;
        if(msg_id == PTP_FOLLOW_UP)
        {
            pc.t1.s = HTONS_32(msg->s);
            pc.t1.ns = HTONS_32(msg->ns);
            pc.save_ts = &pc.t3;
            prepare_ptp_msg(msg, PTP_DELAY_REQ, PTP_LEN_DELAY_REQ, CTRL_DELAY_REQ, 0);
            pc.next_msg_id = PTP_DELAY_RESP;
            return PTP_LEN_DELAY_REQ;
        }
        if(msg_id == PTP_DELAY_RESP)
        {
            ptpmsg_delay_resp_t *resp = (ptpmsg_delay_resp_t*)(data + sizeof(ptpmsg_t));
            if(pc.source_port_id != HTONS_16(resp->source_port_id))
                return 0;
            if(mymemcmp((char*)pc.clock_id, (char*)resp->clock_id, 8))
                return 0;
            pc.t4.s = HTONS_32(msg->s);
            pc.t4.ns = HTONS_32(msg->ns);
            ptp_calculate_delay_and_offset();
            pc.next_msg_id = PTP_SYNC;
            return 0;
        }
        return 0;
    }
    if(pc.state == PTP_P2P)
    {
        if(pc.wait == 0)
        {
            if(msg_id == PTP_PDELAY_REQ)
            {
                pc.next_msg_id = PTP_PDELAY_RESP_FOLLOW_UP;
                return handle_pdelay_req(msg, sz);
            }
            return 0;
        }
        if(pc.wait == 1)
        {
            if(msg_id == PTP_PDELAY_RESP)
            {
                pc.next_msg_id = PTP_PDELAY_RESP_FOLLOW_UP;
                return handle_pdelay_resp(msg, sz);
            }
            if(msg_id == PTP_PDELAY_RESP_FOLLOW_UP)
            {
                pc.next_msg_id = PTP_SYNC;
                return handle_pdelay_resp_follow_up(msg, sz);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

uint16_t myip_ptpd_evt_io(uint8_t *data, uint16_t sz)
{
    return ptpmsg_handle(data, sz);
}

uint16_t myip_ptpd_msg_io(uint8_t *data, uint16_t sz)
{
    return ptpmsg_handle(data, sz);
}

uint16_t myip_ptpd_set_port_id(uint16_t port_id)
{
    pc.next_msg_id = PTP_SYNC;
    pc.source_port_id = port_id;
    return pc.source_port_id;
}

uint16_t myip_ptpd_get_port_id(void)
{
    return pc.source_port_id;
}

void myip_ptpd_get_clock_id(uint8_t *buf)
{
    mymemcpy(buf, pc.clock_id, 8);
}

void myip_ptpd_get_offset(ptpdt_t *offset)
{
    mymemcpy(offset, &pc.offset, sizeof(ptpdt_t));
}

// alpha = 1/2^s
// y[1] = x[0]
// y[n] = alpha * x[n-1] + (1-alpha) * y[n-1]
void myip_ptpd_filter_offset(ptpdt_t *dt, ptpdt_t *dtf)
{
    dtf->ns += dt->ns;
    dtf->ns /= 2;
}

#define PI_PA   2
#define PI_PI   16
#define PI_INT_MAX  1000000
// suppose log_sync_interval = 0
void myip_ptpd_adjust_freq(ptpdt_t *dt)
{
    int32_t pi_prop = 0, pi_sum = 0;
    static int32_t pi_int = 0;
    pi_prop = dt->ns / PI_PA;
    pi_int += dt->ns / PI_PI;

    if(pi_int < -PI_INT_MAX)
        pi_int = -PI_INT_MAX;
    if(pi_int > PI_INT_MAX)
        pi_int = PI_INT_MAX;

    pi_sum = pi_prop + pi_int;
#if 0
    dbg_send_int2("prop", pi_prop);
    dbg_send_int2("int", pi_int);
    dbg_send_int2("sum", pi_sum);
#endif
    eth_ptpfreq_adjust(pi_sum);
}

// delay = [(t2-t1)+(t4-t3)]/2 = (t2/2+t4/2)-(t1/2+t3/2)
// -offset = -[(t2-t1)-(t4-t3)]/2 = (t1/2+t4/2)-(t2/2+t3/2)
static void ptp_calculate_delay_and_offset(void)
{
#if 1
    //dbg_send_int2("t1.s", pc.t1.s);
    dbg_send_int2("t1.ns", pc.t1.ns);
    //dbg_send_int2("t2.s", pc.t2.s);
    dbg_send_int2("t2.ns", pc.t2.ns);
    //dbg_send_int2("t3.s", pc.t3.s);
    dbg_send_int2("t3.ns", pc.t3.ns);
    //dbg_send_int2("t4.s", pc.t4.s);
    dbg_send_int2("t4.ns", pc.t4.ns);
    ptpdt_t dt;
    ptpts_diff(&pc.t4, &pc.t1, &dt);
    //dbg_send_int2("t4 - t1", dt.ns);
    ptpts_diff(&pc.t3, &pc.t2, &dt);
    //dbg_send_int2("t3 - t2", dt.ns);
#endif
    ptpts_div2(&pc.t1);
    ptpts_div2(&pc.t2);
    ptpts_div2(&pc.t3);
    ptpts_div2(&pc.t4);
    ptpts_t sum1, sum2;
    ptpts_sum(&pc.t2, &pc.t4, &sum1);
    ptpts_sum(&pc.t1, &pc.t3, &sum2);
    ptpts_diff(&sum1, &sum2, &pc.delay);
    dbg_send_int2("delay.ns", pc.delay.ns);

#if 0
    dbg_send_int2("t1.s", pc.t1.s);
    dbg_send_int2("t1.ns", pc.t1.ns);
    dbg_send_int2("t2.s", pc.t2.s);
    dbg_send_int2("t2.ns", pc.t2.ns);
    dbg_send_int2("t3.s", pc.t3.s);
    dbg_send_int2("t3.ns", pc.t3.ns);
    dbg_send_int2("t4.s", pc.t4.s);
    dbg_send_int2("t4.ns", pc.t4.ns);
#endif
    ptpts_sum(&pc.t1, &pc.t4, &sum1);
    ptpts_sum(&pc.t2, &pc.t3, &sum2);
#if 0
    dbg_send_int2("sum1.s", sum1.s);
    dbg_send_int2("sum1.ns", sum1.ns);
    dbg_send_int2("sum2.s", sum2.s);
    dbg_send_int2("sum2.ns", sum2.ns);
#endif
    ptpdt_t offset;
    ptpts_diff(&sum1, &sum2, &offset);
#if 0
    dbg_send_int2("delay.s", delay.s);
    dbg_send_int2("delay.ns", delay.ns);
#endif
    //dbg_send_int2("offset.s", offset.s);
    //dbg_send_int2("offset.ns", offset.negative ? -offset.ns : offset.ns);
    //dbg_send_int2("offset.negative", offset.negative);
    if(offset.s != 0)
    {
        dbg_send_str3("----------coarse update----------", 1);
        eth_ptpts_coarse_update(&offset);
        pc.offset.s = 0;
        pc.offset.ns = 0;
    }
    else
    {
        dbg_send_int2("offset.ns(raw)", offset.ns);
        myip_ptpd_filter_offset(&offset, &pc.offset);
        dbg_send_int2("offset.ns(filt)", pc.offset.ns);
        myip_ptpd_adjust_freq(&pc.offset);
    }
}

uint32_t ptp_ss2ns(uint32_t ss)
{
    uint64_t val = ss * 1000000000ll;
    val >>= 31;
    return (uint32_t)val;
}

uint32_t ptp_ns2ss(int32_t ns)
{
    if(ns < 0)
        ns = -ns;
    uint64_t val = ns * 0x80000000ll;
    val /= 1000000000;
    return (uint32_t)val;
}

void ptpts_sum(const ptpts_t *t1, const ptpts_t *t2, ptpts_t *sum)
{
    sum->s = t1->s + t2->s;
    sum->ns = t1->ns + t2->ns;
    if(sum->ns > 1000000000)
    {
        sum->s += 1;
        sum->ns -= 1000000000;
    }
}

void ptpts_diff(const ptpts_t *t1, const ptpts_t *t2, ptpdt_t *dt)
{
    if((t1->s < t2->s) || ((t1->s == t2->s) && (t1->ns < t2->ns)))
    {
        ptpts_diff(t2, t1, dt);
        dt->ns = -dt->ns;
        return;
    }
    dt->s = t1->s;
    uint32_t ns = t1->ns;
    if(ns < t2->ns)
    {
        dt->s -= 1;
        ns += 1000000000;
    }
    dt->s -= t2->s;
    ns -= t2->ns;
    dt->ns = (int32_t)ns;
}

void ptpts_div2(ptpts_t *ts)
{
    if(ts->s % 2)
    {
        ts->s -= 1;
        ts->ns += 1000000000;
    }
    ts->s /= 2;
    ts->ns /= 2;
}

uint8_t pdelay_ipaddr[4];

uint16_t myip_ptpd_pdelay(uint8_t *ipaddr, ptpdt_t *dt)
{
    if(ipaddr)
    {
        myip_ptpd_set_state(PTP_P2P_SLAVE);
        pc.wait = 1;
        mymemcpy(pdelay_ipaddr, ipaddr, 4);
        return 1;
    }
    else
    {
        if((pc.wait == 1) && (pc.state == PTP_OFF))
        {
            dt->s = pc.delay.s;
            dt->ns = pc.delay.ns;
            pc.wait = 0;
            return 0;
        }
    }
    return 1;
}

static void prepare_ptp_frm(ETH_FRAME *frm, uint8_t msg_id, uint16_t msg_len)
{
    UDP_FRAME* ufrm = (UDP_FRAME*)frm;
    preamble3 *p = &ufrm->p;
    const uint8_t *dst_ip_addr = mcast_ipaddr;
    if((msg_id == PTP_PDELAY_REQ) || (msg_id == PTP_PDELAY_RESP) || (msg_id == PTP_PDELAY_RESP_FOLLOW_UP))
        dst_ip_addr = p2p_ipaddr;
    myip_make_ip_frame((IP_FRAME*)frm, dst_ip_addr, IPH_SZ + UDPH_SZ + msg_len, ip_counter++, UDP_PROTO);
    mymemcpy(p->dst, mcast_macaddr, 6);
    ufrm->p.src_port = HTONS_16(UDP_PORT_PTP_EVT);
    ufrm->p.dst_port = HTONS_16(UDP_PORT_PTP_EVT);
    ufrm->p.ttl = 1;
    ufrm->len = HTONS_16(UDPH_SZ + msg_len);
    ufrm->cksum = 0;

}
static void prepare_ptp_frm_msg(ETH_FRAME *frm, uint8_t msg_id, uint16_t msg_len, uint8_t ctrl, ptpts_t *pts)
{
    prepare_ptp_frm(frm, msg_id, msg_len);

    char *ptr = (char*)frm;
    ptr += MACH_SZ + IPH_SZ + UDPH_SZ;
    ptpmsg_t *msg = (ptpmsg_t*)ptr;
    prepare_ptp_msg(msg, msg_id, msg_len, ctrl, pts);
}

uint16_t myip_ptpd_frm_handler(ETH_FRAME *frm, uint16_t sz, uint16_t con_index)
{
    if(sz)
    {
        UDP_FRAME* ufrm = (UDP_FRAME*)frm;
        if(con_index >= CON_TABLE_SZ)
            return 0;
        if(ufrm->p.proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].proto != UDP_PROTO)
            return 0;
        if(con_table[con_index].port != HTONS_16(ufrm->p.dst_port))
            return 0;
        uint8_t *data_ptr;
        uint16_t data_sz = myip_udp_data(ufrm, sz, &data_ptr);
        ptpmsg_t *msg = (ptpmsg_t*)data_ptr;
        data_sz = con_table[con_index].con_handler_ptr(data_ptr, data_sz);
        if(data_sz)
        {
            prepare_ptp_frm(frm, msg->msg_id, data_sz);
            return MACH_SZ + IPH_SZ + UDPH_SZ + data_sz;
        }
        return 0;
    }
    if(pc.wait == 1)
    {
        if(pc.state == PTP_P2P_SLAVE)
        {
            if(pc.next_msg_id == PTP_PDELAY_REQ)
            {
                prepare_ptp_frm_msg(frm, PTP_PDELAY_REQ, PTP_LEN_PDELAY_REQ, CTRL_OTHER, 0);

                pc.next_msg_id = PTP_PDELAY_RESP;
                pc.save_ts = &pc.t1;
                return MACH_SZ + IPH_SZ + UDPH_SZ + PTP_LEN_PDELAY_REQ;
            }
        }
        return 0;
    }
    if(pc.state & PTP_MASTER)
    {
        if(pc.next_msg_id == PTP_SYNC)
        {
            if(pc.sync_ts == uptime())
                return 0;

            pc.sequence_id += 1;

            prepare_ptp_frm_msg(frm, PTP_SYNC, PTP_LEN_SYNC, CTRL_SYNC, 0);

            pc.next_msg_id = PTP_FOLLOW_UP;
            pc.save_ts = &pc.t1;
            pc.sync_ts = uptime();
            return MACH_SZ + IPH_SZ + UDPH_SZ + PTP_LEN_SYNC;
        }
        if(pc.next_msg_id == PTP_FOLLOW_UP)
        {
            prepare_ptp_frm_msg(frm, PTP_FOLLOW_UP, PTP_LEN_FOLLOW_UP, CTRL_FOLLOW_UP, &pc.t1);

            pc.next_msg_id = PTP_SYNC;
            pc.save_ts = 0;
            return MACH_SZ + IPH_SZ + UDPH_SZ + PTP_LEN_FOLLOW_UP;
        }
    }
    if((pc.wait == 0) && (pc.next_msg_id == PTP_PDELAY_RESP_FOLLOW_UP))
    {
        prepare_ptp_frm_msg(frm, PTP_PDELAY_RESP_FOLLOW_UP, PTP_LEN_PDELAY_RESP_FOLLOW_UP, CTRL_OTHER, &pc.t3);

        char *ptr = (char*)frm;
        ptr += MACH_SZ + IPH_SZ + UDPH_SZ + sizeof(ptpmsg_t);
        ptpmsg_delay_resp_t *resp = (ptpmsg_delay_resp_t*)(ptr);
        mymemcpy(resp->clock_id, pc.req.clock_id, 8);
        resp->source_port_id = pc.req.source_port_id;

        pc.next_msg_id = PTP_PDELAY_REQ;
        pc.save_ts = 0;
        return MACH_SZ + IPH_SZ + UDPH_SZ + PTP_LEN_PDELAY_RESP_FOLLOW_UP;
    }
    return 0;
}

