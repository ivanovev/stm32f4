
#include <main.h>
#ifdef MY_UART
#include "uart/uart.h"
#endif
#ifdef MY_USB
#include "usb/cdc/usb.h"
#endif

int main(void)
{
    myinit();

#if 1
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
#else
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
#endif

    io_send_str3("pcl_test", 1);
    for (;;)
    {
        pcl_io();
    }
}

