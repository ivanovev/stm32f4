
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

typedef struct {
    uint16_t port;
    uint16_t ackn;
    uint16_t mode;
    uint32_t crc;
    uint32_t start, end;
} TFTPD_STATE;

void        myip_tftpd_init(void);
uint16_t    myip_tftpd_con_handler(uint8_t *data, uint16_t sz);
uint16_t    myip_tftpd_frm_handler(ETH_FRAME *frm, uint16_t sz, uint16_t con_index);
uint16_t    myip_tftpd_io_flash_tx(uint32_t sz, uint32_t offset);
uint16_t    myip_tftpd_io_flash_rx(uint32_t sz, uint32_t offset, uint8_t reset);

#endif

