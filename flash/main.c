
#include <main.h>
#include "flash.h"
#include "uart/uart.h"

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
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    flash_write_otp_data();
    flash_write_otp_lock();
    led_on();
    //io_send_int2("flash_fsz0", flash_fsz0());
    //io_send_int2("flash_fsz1", flash_fsz1());
    //io_send_hex2("flash_crc1", flash_crc1());
    for (;;) {
    }
}

