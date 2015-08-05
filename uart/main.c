
#include <main.h>

int main(void)
{
    myinit();
#if 0
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    uart_send_str2("123");
#endif
    uint32_t i = 0;
    for (;;)
    {
        //dbg_send_str3("uart_test", 1);
        //io_echo();
        HAL_Delay(1000);
        led_toggle();
        //uart_send_int(i++);
    }
}

