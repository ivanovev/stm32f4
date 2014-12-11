
#include <main.h>
#include "flash.h"

static void flash_write_otp_data(void)
{
    uint32_t addr = 0x1FFF79E0;
    uint32_t data = 0x00000001;
    HAL_FLASH_Unlock();
    HAL_FLASH_Program(TYPEPROGRAM_WORD, addr & 0xFFFFFFFC, data);
    HAL_FLASH_Lock();
}

static void flash_write_otp_lock(void)
{
    uint32_t addr = 0x1FFF7A0F;
    uint8_t data = 0x00;
    HAL_FLASH_Unlock();
    HAL_FLASH_Program(TYPEPROGRAM_BYTE, addr, data);
    HAL_FLASH_Lock();
}

int main(void)
{
    myinit();
    flash_write_otp_data();
    flash_write_otp_lock();
    led_on();
    //dbg_send_int2("flash_fsz0", flash_fsz0());
    //dbg_send_int2("flash_fsz1", flash_fsz1());
    //dbg_send_hex2("flash_crc1", flash_crc1());
    for (;;) {
    }
}

