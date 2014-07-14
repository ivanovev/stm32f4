
#include "main.h"

extern void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    usb_init();
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
    int sz;
    char buf[IO_BUF_SZ];
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

