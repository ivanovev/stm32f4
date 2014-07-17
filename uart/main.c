
#include "main.h"
#include "uart.h"
#include "gpio/led.h"
#include "gpio/btn.h"

extern void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    led_init();
    btn_init();
    uart_init();
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    uart_send_str3("uart_test", 1);
    char buf[IO_BUF_SZ];
    uint32_t sz;
    for (;;)
    {
#if 1
        sz = io_recv_str(buf);
        if(sz)
        {
            if(buf[0] != '\n')
                io_send_str3(buf, 0);
            io_prompt(0);
        }
#else
        HAL_Delay(1000);
        if(btn_status())
        {
            uart_send_str("2", 1);
            led_on();
        }
        else
        {
            led_off();
        }
#endif
    }
}

