
#include "main.h"
#include "uart.h"

extern void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    uart_init();
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    char buf[IO_BUF_SZ];
    uint32_t sz;
    for (;;)
    {
        sz = io_recv_str(buf);
        if(sz)
        {
            if(buf[0] != '\n')
                io_send_str3(buf, 0);
            io_prompt(0);
        }
    }
}

