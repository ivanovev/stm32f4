
#include "main.h"
#include "gpio/led.h"
#ifdef HAL_PCD_MODULE_ENABLED
#include "usb/cdc/usb.h"
#endif
#ifdef HAL_UART_MODULE_ENABLED
#include "uart/uart.h"
#endif

extern void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    led_init();
#ifdef HAL_PCD_MODULE_ENABLED
    usb_init();
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
#else
#ifdef HAL_UART_MODULE_ENABLED
    uart_init();
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
#endif
#endif
    io_send_str3("pcl_test", 1);
    pcl_init();
    char buf[IO_BUF_SZ];
    uint32_t sz;
    for (;;)
    {
        sz = io_recv_str(buf);
        if(sz)
        {
            if(buf[0] != '\n')
            {
                pcl_exec(buf);
                io_send_str3(buf, 0);
                io_prompt(1);
            }
            else
                io_prompt(0);
        }
    }
}

