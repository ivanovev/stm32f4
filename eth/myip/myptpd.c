
#include "myip.h"
#include "myptpd.h"

#define PTP_SYNC        0
#define PTP_DELAY_REQ   1
#define PTP_FOLLOW_UP   8
#define PTP_DELAY_RESP  9

#define PTP_LEN_DELAY_REQ   44

typedef struct ptpclk_t
{
    ptpts_t t1, t2, t3, t4;
    uint16_t sequence_id;
    uint8_t ptp_ok;
} ptpclk_t;

ptpclk_t pc;

void myip_ptpd_init(void)
{
    pc.ptp_ok = 0;
}

static uint16_t handle_sync(ptpmsg_t *msg, uint16_t sz)
{
    eth_ptpts_get(&pc.t2, 0);
    pc.sequence_id = HTONS_16(msg->sequence_id);
    return 0;
}

static uint16_t handle_follow_up(ptpmsg_t *msg, uint16_t sz)
{
    pc.t1.s = HTONS_32(msg->s);
    pc.t1.ns = HTONS_32(msg->ns);
    msg->msg_id &= 0xF0;
    msg->msg_id |= PTP_DELAY_REQ;
    msg->msg_len = HTONS_16(PTP_LEN_DELAY_REQ);
    msg->correction[0] = 0;
    msg->correction[1] = 0;
    msg->sequence_id = HTONS_16(pc.sequence_id);
    msg->control_field = PTP_DELAY_REQ;
    msg->log_msg_interval = 0x7F;
    msg->s = HTONS_32(pc.t1.s);
    msg->ns = HTONS_32(pc.t1.ns);

    return PTP_LEN_DELAY_REQ;
}

void myip_ptpd_save_t3(ptpts_t *t)
{
    pc.t3.s = t->s;
    pc.t3.ns = t->ns;
}

static void ptp_calculate_delay_and_offset(void);
static uint16_t handle_delay_resp(ptpmsg_t *msg, uint16_t sz)
{
    static uint16_t counter = 0;
    pc.t4.s = HTONS_32(msg->s);
    pc.t4.ns = HTONS_32(msg->ns);
#if 0
    dbg_send_int2("counter", counter++);
    dbg_send_int2("t1.s", pc.t1.s);
    dbg_send_int2("t1.ns", pc.t1.ns);
    dbg_send_int2("t2.s", pc.t2.s);
    dbg_send_int2("t2.ns", pc.t2.ns);
    dbg_send_int2("t3.s", pc.t3.s);
    dbg_send_int2("t3.ns", pc.t3.ns);
    dbg_send_int2("t4.s", pc.t4.s);
    dbg_send_int2("t4.ns", pc.t4.ns);
    //pc.ptp_ok = 1;
#endif
    ptp_calculate_delay_and_offset();
    return 0;
}

static uint16_t handle_ptp_msg(uint8_t *data, uint16_t sz)
{
    if(pc.ptp_ok == 1)
        return 0;
    ptpmsg_t *msg = (ptpmsg_t*)data;
    uint8_t msg_id = msg->msg_id & 0x0F;
    if(msg_id == PTP_SYNC) // sync
    {
        return handle_sync(msg, sz);
    }
    else if((msg_id == PTP_FOLLOW_UP) && (pc.sequence_id == HTONS_16(msg->sequence_id))) // follow up
    {
        return handle_follow_up(msg, sz);
    }
    else if((msg_id == PTP_DELAY_RESP) && (pc.sequence_id == HTONS_16(msg->sequence_id))) // delay resp
    {
        return handle_delay_resp(msg, sz);
    }
#if 0
    uint32_t s = HTONS_32(msg->s);
    s = s % (24*60*60);
    char buf[32];
    format_time(buf, sizeof(buf), s);
    dbg_send_hex2("evt_s", msg->s);
    dbg_send_str3(buf, 1);
    return 0;
#else
    return 0;
#endif
}

uint16_t myip_ptpd_evt_io(uint8_t *data, uint16_t sz)
{
    if(!sz)
        return 0;
    return handle_ptp_msg(data, sz);
}

uint16_t myip_ptpd_msg_io(uint8_t *data, uint16_t sz)
{
    if(!sz)
        return 0;
    return handle_ptp_msg(data, sz);
}

// alpha = 1/2^s
// y[1] = x[0]
// y[n] = alpha * x[n-1] + (1-alpha) * y[n-1]
int32_t myip_ptpd_filter_offset(ptpdt_t *dt)
{
    static int32_t offset_prev = 0;
    offset_prev += dt->ns;
    offset_prev /= 2;
    return offset_prev;
}

#define PI_PA   2
#define PI_PI   16
#define PI_ADJ_MAX  1000000
void myip_ptpd_adjust_freq(ptpdt_t *dt)
{
    int32_t pi_prop = 0, pi_sum = 0;
    static int32_t pi_int = 0;
    pi_prop = dt->ns / PI_PA;
    if(dt->negative)
    {
        pi_prop = -pi_prop;
        pi_int -= dt->ns / PI_PI;
        if(pi_int < -PI_ADJ_MAX)
            pi_int = -PI_ADJ_MAX;
    }
    else
    {
        pi_int += dt->ns / PI_PI;
        if(pi_int > PI_ADJ_MAX)
            pi_int = PI_ADJ_MAX;
    }
    pi_sum = pi_prop + pi_int;
    //dbg_send_int2("prop", pi_prop);
    //dbg_send_int2("int", pi_int);
    //dbg_send_int2("sum", pi_sum);
    eth_ptpfreq_adjust(pi_sum/1000);
}

// delay = [(t2-t1)+(t4-t3)]/2 = (t2/2+t4/2)-(t1/2+t3/2)
// -offset = -[(t2-t1)-(t4-t3)]/2 = (t1/2+t4/2)-(t2/2+t3/2)
static void ptp_calculate_delay_and_offset(void)
{
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
    ptpts_div2(&pc.t1);
    ptpts_div2(&pc.t2);
    ptpts_div2(&pc.t3);
    ptpts_div2(&pc.t4);
    ptpts_t sum1, sum2;
    ptpts_sum(&pc.t2, &pc.t4, &sum1);
    ptpts_sum(&pc.t1, &pc.t3, &sum2);
    ptpdt_t delay;
    ptpts_diff(&sum1, &sum2, &delay);

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
    dbg_send_int2("offset.ns", offset.negative ? -offset.ns : offset.ns);
    //dbg_send_int2("offset.negative", offset.negative);
    if(offset.s != 0)
    {
        dbg_send_str3("-----------------------------------------update-----------------------------", 1);
        eth_ptpts_update(&offset);
    }
    else
    {
        offset.ns = myip_ptpd_filter_offset(&offset);
        dbg_send_int2("offset.ns(filt)", offset.negative ? -offset.ns : offset.ns);
        myip_ptpd_adjust_freq(&offset);
    }
}

uint32_t ptp_ss2ns(uint32_t ss)
{
    uint64_t val = ss * 1000000000ll;
    val >>= 31;
    return (uint32_t)val;
}

uint32_t ptp_ns2ss(uint32_t ns)
{
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
    dt->negative = 0;
    if((t1->s < t2->s) || ((t1->s == t2->s) && (t1->ns < t2->ns)))
    {
        ptpts_diff(t2, t1, dt);
        dt->negative = 1;
        return;
    }
    dt->s = t1->s;
    dt->ns = t1->ns;
    if(dt->ns < t2->ns)
    {
        dt->s -= 1;
        dt->ns += 1000000000;
    }
    dt->s -= t2->s;
    dt->ns -= t2->ns;
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

