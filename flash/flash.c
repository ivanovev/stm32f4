
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

#if 1
uint8_t flash_write(uint32_t addr, uint32_t data)
{
    if((USER_FLASH_START_ADDR <= addr) && (addr <= (USER_FLASH_END_ADDR - 4)))
    {
        if(HAL_FLASH_Program(TYPEPROGRAM_WORD, addr & 0xFFFFFFFC, data) != HAL_OK)
            return 1;
    }
    else
        return 1;
    return 0;
}
#endif

uint32_t flash_erase2(void)
{
    uint32_t SectorError = 0;
    FLASH_EraseInitTypeDef flash_erase_init;
    flash_erase_init.TypeErase = TYPEERASE_SECTORS;
    flash_erase_init.Sector = FLASH_SECTOR_8;
    flash_erase_init.NbSectors = FLASH_SECTOR_11 - FLASH_SECTOR_8 + 1;
    flash_erase_init.VoltageRange = VOLTAGE_RANGE_3;

    if(HAL_FLASHEx_Erase(&flash_erase_init, &SectorError) != HAL_OK)
        return SectorError;

    return 0;
}

uint8_t flash_write_data(uint8_t *data, uint16_t sz, uint32_t addr)
{
    if(addr & 3)
        return 0;
    uint16_t i, j;
    for(i = 0; i < sz; i += 4)
    {
        if((sz - i) <= 4)
        {
            for(j = sz; j % 4; j++)
                data[j] = 0xFF;
        }
        if(flash_write(addr + i, (uint32_t)data[i]))
            return 1;
    }
    return 0;
}

