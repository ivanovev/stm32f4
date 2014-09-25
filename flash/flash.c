
#include <main.h>
#include "flash.h"

static uint32_t flash_fsz(uint32_t *start, uint32_t *end)
{
    static uint32_t start_addr = 0;
    if(start_addr == 0)
        start_addr = (uint32_t)start;
    if((end - start) < 4)
    {
        uint8_t *ptr8 = (uint8_t*)end;
        while(--ptr8 > start)
            if(*(ptr8-1) != 0xFF)
                break;
        end = ptr8 - start_addr;
        start_addr = 0;
        return end;
    }
    uint32_t *mid = (uint32_t*)(((uint32_t)start + (uint32_t)end)/2);
    mid -= 1;
    end -= 1;
    if((*mid == 0xFFFFFFFF) && (*end == 0xFFFFFFFF))
        return flash_fsz(start, mid + 1);
    else
        return flash_fsz(mid + 1, end + 1);
}

uint32_t flash_fsz1(void)
{
    return flash_fsz(USER_FLASH_START_ADDR, USER_FLASH_MID_ADDR);
}

uint32_t flash_fsz2(void)
{
    return flash_fsz(USER_FLASH_MID_ADDR, USER_FLASH_END_ADDR);
}

