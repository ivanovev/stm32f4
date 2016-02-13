
#include <main.h>

int main(void)
{
    myinit();
    led_on();
    uint8_t data[] = {0x00, 0xFF};
    spi_send(SPIn, 0, 0, data, sizeof(data));
    for (;;)
    {
        //dbg_send_str3("uart_test", 1);
        //io_echo();
        HAL_Delay(1000);
        led_toggle();
        //uart_send_int(i++);
    }
}

