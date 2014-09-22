
#include "main.h"

int main(void)
{
    myinit();
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
    for (;;)
    {
        io_echo();
    }
}

