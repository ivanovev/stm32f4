
#include <main.h>
#include "eth/eth.h"
#include "eth/myip/mytcp.h"
#include "eth/myip/mytelnetd.h"
#include "core_cm4.h"

volatile uint8_t reset = 0;

int main(void)
{
    myinit();
    io_recv_str_ptr = telnetd_recv_str;
    io_send_str_ptr = telnetd_send_str;
    for (;;)
    {
        //eth_io();
        pcl_io();
        if(reset && myip_tcp_con_closed())
            break;
    }
    uart_send_int2("reset", reset);
    if(reset == 1)
    {
        NVIC_SystemReset();
    }
    if(reset == 2)
    {
        uart_send_int2("copy10", flash_copy10());
        NVIC_SystemReset();
    }
    for(;;)
    {
        pcl_io();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    eth_io();
}

