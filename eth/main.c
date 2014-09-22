
#include <main.h>
#include "eth/eth.h"
#include "eth/myip/mytelnetd.h"

int main(void)
{
    myinit();
    io_recv_str_ptr = telnetd_recv_str;
    io_send_str_ptr = telnetd_send_str;
    for (;;)
    {
        //eth_io();
        pcl_io();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    eth_io();
}

