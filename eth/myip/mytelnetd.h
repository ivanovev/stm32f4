
#ifndef __MYTELNETD_H__
#define __MYTELNETD_H__

#include "myip.h"
#include "mytcp.h"
#include "util/queue.h"

#pragma pack(1)
typedef struct {
    Queue qi;
    Queue qo;
} TELNETD_STATE;

void        myip_telnetd_init(void);
uint16_t    myip_telnetd_io(uint8_t *data, uint16_t sz);

uint16_t    telnetd_con_handler(ETH_FRAME *frm, uint16_t sz);

uint16_t    telnetd_recv_str(char *buf);
void        telnetd_send_str(const char *str, uint16_t len);

#endif

