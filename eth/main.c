
#include <main.h>
#include "eth/eth.h"
#include "eth/myip/mytcp.h"
#ifdef ENABLE_TELNET
#include "eth/myip/telnet/mytelnetd.h"
#endif
#include "core_cm4.h"
#include "flash/flash.h"

volatile uint8_t reset = 0;

int main(void)
{
    myinit();
#ifdef ENABLE_TELNET
    io_recv_str_ptr = telnetd_recv_str;
    io_send_str_ptr = telnetd_send_str;
#endif
    for (;;)
    {
#ifdef ENABLE_PCL
        pcl_io();
#endif
#ifdef ENABLE_VFD
        vfd_upd();
#endif
        eth_io();
#if 0
        if(myip_tcp_con_closed())
        {
            if((reset == RESET_FWUPG) || (reset == RESET_REBOOT))
                break;
        }
#endif
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

