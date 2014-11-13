
#ifndef __MYPTPD_H__
#define __MYPTPD_H__

#include <main.h>

#define PTP_COARSE      0
#define PTP_FINE        1

#pragma pack(1)
typedef struct ptpmsg_t
{
    uint8_t msg_id;
    uint8_t ptp_version;
    uint16_t msg_len;
    uint8_t domain_num;
    uint8_t reserved1;
    uint16_t flags;
    uint32_t correction[2];
    uint8_t reserved2[4];
    uint8_t source_port_id[10];
    uint16_t sequence_id;
    uint8_t control_field;
    uint8_t log_msg_interval;
    uint16_t s0;
    uint32_t s;
    uint32_t ns;
} ptpmsg_t;


void        myip_ptpd_init(void);
uint16_t    myip_ptpd_evt_io(uint8_t *data, uint16_t sz);
uint16_t    myip_ptpd_msg_io(uint8_t *data, uint16_t sz);
void        myip_ptpd_save_t3(ptpts_t *t);

#endif

