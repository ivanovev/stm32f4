
#include "mydatad.h"
#include "uart/uart.h"
#include "flash/flash.h"

#define DATAD_IDLE 0
#define DATAD_FLASH_TX 1
#define DATAD_FLASH_RX 2

DATAD_STATE dds;
extern volatile uint8_t reset;

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
        flash_write_array(dds.start, data, sz);
        dds.start += sz;
        if(dds.start >= dds.end)
        {
            uint8_t eof[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            flash_write_array(dds.start, eof, sizeof(eof));
            //uart_send_hex2("myip_datad_io.end_ok", dds.start);
            dds.state = DATAD_IDLE;
            if(dds.reset)
            {
                //uart_send_hex2("myip_datad_io.reset", dds.reset);
                reset = dds.reset;
            }
            else
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

uint16_t myip_datad_io_flash_rx(uint32_t sz, uint32_t offset, uint8_t reset)
{
    HAL_FLASH_Unlock();
    dds.start = USER_FLASH_START_ADDR + offset;
    dds.end = dds.start + sz;
    dds.state = DATAD_FLASH_RX;
    dds.reset = reset;
    //uart_send_hex2("myip_datad_io.start", dds.start);
    //uart_send_hex2("myip_datad_io.end", dds.end);
    return sz;
}

