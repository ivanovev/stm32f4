
#include <main.h>
#ifdef ENABLE_UART
#include "uart/uart.h"
#endif
#ifdef ENABLE_USB
#include "usb/cdc/usb.h"
#endif

volatile uint8_t main_evt = 0;

int main(void)
{
    myinit();

#ifdef ENABLE_UART
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
#else
#ifdef ENABLE_USB
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
#endif
#endif

    dbg_send_str3("pcl_test", 1);
    for (;;)
    {
        pcl_io();
    }
}

