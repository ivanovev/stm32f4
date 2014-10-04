
#include <main.h>
#include "flash.h"
#include "uart/uart.h"

int main(void)
{
    myinit();
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    io_send_int2("flash_fsz0", flash_fsz0());
    io_send_int2("flash_fsz1", flash_fsz1());
    io_send_hex2("flash_crc1", flash_crc1());
    for (;;) {
    }
}

