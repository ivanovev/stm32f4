
#include "mydatad.h"
#include "uart/uart.h"
#include "flash/flash.h"

#define DATAD_IDLE 0
#define DATAD_FLASH_TX 1
#define DATAD_FLASH_RX 2

DATAD_STATE dds;

void myip_datad_init(void)
{
    dds.start = 0;
    dds.end = 0;
    dds.state = DATAD_IDLE;
}

uint16_t myip_datad_io(uint8_t *data, uint16_t sz)
{
    if(dds.state == DATAD_IDLE)
        return 0;
    uint32_t chunk_sz = 512;
    if(dds.state == DATAD_FLASH_TX)
    {
        if(chunk_sz >= (dds.end - dds.start))
        {
            chunk_sz = (dds.end - dds.start);
            dds.state = DATAD_IDLE;
        }
        mymemcpy(data, (uint8_t*)dds.start, chunk_sz);
        dds.start += chunk_sz;
        return chunk_sz;
    }
    if((dds.state == DATAD_FLASH_RX) && sz)
    {
        uart_send_int2("myip_datad_io.sz", sz);
        uint16_t i;
        for(i = 0; i < sz; i += 4)
            flash_write(dds.start + i, (uint32_t)data[i]);
        dds.start += sz;
        if(dds.start >= dds.end)
        {
            dds.state = DATAD_IDLE;
            HAL_FLASH_Lock();
        }
    }
    return 0;
}

uint16_t myip_datad_io_flash_tx(uint32_t sz, uint32_t offset)
{
    dds.start = USER_FLASH_START_ADDR + offset;
    dds.end = dds.start + sz;
    dds.state = DATAD_FLASH_TX;
    return sz;
}

uint16_t myip_datad_io_flash_rx(uint32_t sz, uint32_t offset)
{
    HAL_FLASH_Unlock();
    dds.start = USER_FLASH_START_ADDR + offset;
    dds.end = dds.start + sz;
    dds.state = DATAD_FLASH_RX;
    return sz;
}

