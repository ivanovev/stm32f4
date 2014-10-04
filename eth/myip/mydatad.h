
#ifndef __MYDATAD_H__
#define __MYDATAD_H__

#include "myip.h"
#include "mytcp.h"

#pragma pack(1)
typedef struct {
    uint32_t start, end;
    uint8_t state;
    uint8_t reset;
} DATAD_STATE;

void        myip_datad_init(void);
uint16_t    myip_datad_io(uint8_t *data, uint16_t sz);
uint16_t    myip_datad_io_flash_tx(uint32_t sz, uint32_t offset);
uint16_t    myip_datad_io_flash_rx(uint32_t sz, uint32_t offset, uint8_t reset);

#endif

