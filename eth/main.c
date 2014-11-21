
#include <main.h>
#include "eth/eth.h"
#include "eth/myip/mytcp.h"
#include "eth/myip/mytelnetd.h"
#include "core_cm4.h"
#include "flash/flash.h"

volatile uint8_t reset = 0;

int main(void)
{
    myinit();
    io_recv_str_ptr = telnetd_recv_str;
    io_send_str_ptr = telnetd_send_str;
    for (;;)
    {
        //eth_io();
#ifdef ENABLE_PCL
        pcl_io();
#endif
#ifdef ENABLE_VFD
        vfd_upd();
#endif
        eth_io();
        if(myip_tcp_con_closed())
        {
            if((reset == RESET_FWUPG) || (reset == RESET_REBOOT))
                break;
        }
    }
    mydeinit();
    dbg_send_int2("reset", reset);

#ifdef ENABLE_FLASH
    if(reset == RESET_FWUPG)
    {
        flash_copy10();
    }
#endif
    NVIC_SystemReset();
}

#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    eth_io();
}
#endif

