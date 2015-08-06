
#include <main.h>

#define SDIO_BUF_SZ IO_BUF_SZ

int main(void)
{
    myinit();
    led_on();
    uint8_t buf[SDIO_BUF_SZ];
    unsigned int i=0;
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    dbg_send_str2("test");
    for(i = 0; i < SDIO_BUF_SZ; i++)
    {
        buf[i] = (uint8_t)(i % 256);
    }
    //sdio_write(buf, SDIO_BUF_SZ);
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

