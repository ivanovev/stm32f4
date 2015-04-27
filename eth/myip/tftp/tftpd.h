
#ifndef __TFTPD_H__
#define __TFTPD_H__

#include "myip.h"

#define TFTP_PORT 69

#pragma pack(1)
typedef struct {
    uint16_t opcode;
    uint8_t data[];
} TFTP_PKT;

typedef struct {
    uint16_t opcode;
    uint16_t ackn;
} TFTP_ACK_PKT;

typedef struct {
    uint16_t opcode;
    uint16_t ackn;
    uint8_t data[];
} TFTP_DATA_PKT;

typedef struct {
    uint16_t opcode;
    uint16_t e_code;
    char    e_msg[];
} TFTP_ERR_PKT;

void        myip_tftpd_init(void);
uint16_t    myip_tftpd_con_handler(uint8_t *in, uint16_t sz, uint8_t *out);

#endif

