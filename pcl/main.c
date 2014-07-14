
#include "usb/cdc/usb.h"
#include "main.h"

extern void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    usb_init();
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
    pcl_init();
    char buf[IO_BUF_SZ];
    uint32_t sz;
    for (;;)
    {
        sz = io_recv_str(buf);
        if(sz)
        {
#if 0
            if(buf[0] != '\n')
                io_send_str3(buf, 0);
            io_prompt();
#else
            if(buf[0] != '\n')
            {
                pcl_exec(buf);
                io_send_str3(buf, 0);
                io_prompt(1);
            }
            else
                io_prompt(0);
#endif
        }
    }
}

