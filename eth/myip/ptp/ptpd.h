
#ifndef __PTPD_H__
#define __PTPD_H__

#include <main.h>
#include "eth/eth.h"

#define PTP_TIMn                2
#define PTP_TIMx                JOIN(TIM, PTP_TIMn)
#define PTP_TIMx_CLK_ENABLE     JOIN3(__TIM, PTP_TIMn, _CLK_ENABLE)
#define PTP_TIMx_IRQn           JOIN3(TIM, PTP_TIMn, _IRQn)
#define PTP_TIMx_CH1_GPIO       A
#define PTP_TIMx_CH1_PIN        5
#define PTP_TIMx_AF             GPIO_AF1_TIM2

#define PTP_COARSE      0
#define PTP_FINE        1

#define PTP_OFF         0
#define PTP_E2E         (1 << 0)
#define PTP_P2P         (1 << 1)
#define PTP_MASTER      (1 << 2)
#define PTP_SLAVE       (1 << 3)
#define PTP_E2E_MASTER  (PTP_E2E | PTP_MASTER)
#define PTP_E2E_SLAVE   (PTP_E2E | PTP_SLAVE)
#define PTP_P2P_MASTER  (PTP_P2P | PTP_MASTER)
#define PTP_P2P_SLAVE   (PTP_P2P | PTP_SLAVE)

#define PTP_EVT_PORT    319
#define PTP_MSG_PORT    320

#pragma pack(1)
typedef struct ptphdr_t
{
    uint8_t msg_id;
    uint8_t ptp_version;
    uint16_t msg_len;
    uint8_t domain_num;
    uint8_t reserved1;
    uint16_t flags;
    uint32_t correction[2];
    uint8_t reserved2[4];
    uint8_t clock_id[8];
    uint16_t source_port_id;
    uint16_t sequence_id;
    uint8_t control_field;
    uint8_t log_msg_interval;
    uint16_t s0;
    uint32_t s;
    uint32_t ns;
    uint8_t req_clock_id[8];
    uint16_t req_source_port_id;
} ptphdr_t;

typedef struct {
    machdr_t mac;
    iphdr_t ip;
    udphdr_t udp;
    ptphdr_t ptp;
} ptpfrm_t;

typedef struct ptpts_t {
    uint32_t s;
    uint32_t ns;
} ptpts_t;

typedef struct ptpdt_t {
    uint32_t s;
    int32_t ns; // ns holds sign
} ptpdt_t;

void        myip_ptpd_init(void);
uint16_t    myip_ptpd_evt_io(uint8_t *data, uint16_t sz);
uint16_t    myip_ptpd_msg_io(uint8_t *data, uint16_t sz);

uint16_t    myip_ptpd_get_port_id(void);
uint16_t    myip_ptpd_set_port_id(uint16_t port_id);
void        myip_ptpd_get_clock_id(uint8_t *buf);
void        myip_ptpd_get_offset(ptpdt_t *offset);
uint16_t    myip_ptpd_get_state(void);
void        myip_ptpd_set_state(uint16_t state);

void        myip_ptpd_save_ts(ptpts_t *t);
void        myip_ptpd_adjust_freq(ptpdt_t *dt);

uint16_t    myip_ptpd_frm_handler(ethfrm_t *frm, uint16_t sz, uint16_t con_index);
uint16_t    myip_ptpd_pdelay(uint8_t *ipaddr, ptpdt_t *dt);

uint32_t    ptp_ss2ns(uint32_t ss);
uint32_t    ptp_ns2ss(int32_t ns);
void        ptpts_div2(ptpts_t *t);
void        ptpts_sum(const ptpts_t *t1, const ptpts_t *t2, ptpts_t *sum);
void        ptpts_diff(const ptpts_t *t1, const ptpts_t *t2, ptpdt_t *dt);

#define ETH_PPS_OUT_GPIO B
#define ETH_PPS_OUT_PIN 5

void        eth_ptp_start(ETH_HandleTypeDef *pheth, uint16_t update_method);
int32_t     eth_ptp_ppsfreq(int32_t ppsfreq);

void        eth_ptpclk_time(ptpts_t *ts);
uint32_t    eth_ptpclk_seconds(void);

void        eth_ptpts_get(ptpts_t *pts, volatile ETH_DMADescTypeDef *pdmadesc);
void        eth_ptpts_now(ptpts_t *pts);
void        eth_ptpts_coarse_update(ptpdt_t *dt);
void        eth_ptpts_rx(ptpts_t *pts, ETH_DMADescTypeDef *dmarxdesc);

void        eth_ptpfreq_adjust(int32_t adj);

#endif

