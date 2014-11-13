
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

static uint16_t handle_delay_resp(ptpmsg_t *msg, uint16_t sz)
{
    static uint16_t counter = 0;
    dbg_send_int2("counter", counter++);
    pc.t4.s = HTONS_32(msg->s);
    pc.t4.ns = HTONS_32(msg->ns);
    dbg_send_int2("t1.s", pc.t1.s);
    dbg_send_int2("t1.ns", pc.t1.ns);
    dbg_send_int2("t2.s", pc.t2.s);
    dbg_send_int2("t2.ns", pc.t2.ns);
    dbg_send_int2("t3.s", pc.t3.s);
    dbg_send_int2("t3.ns", pc.t3.ns);
    dbg_send_int2("t4.s", pc.t4.s);
    dbg_send_int2("t4.ns", pc.t4.ns);
    //pc.ptp_ok = 1;
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

