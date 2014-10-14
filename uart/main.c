
#include <main.h>

int main(void)
{
    myinit();
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
    dbg_send_str3("uart_test", 1);
    for (;;)
    {
        io_echo();
    }
}

