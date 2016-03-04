
#include <main.h>
#ifdef ENABLE_ETH
#include "eth/eth.h"
#include "eth/mdio.h"
#include "eth/myip/mytcp.h"
#include "eth/myip/telnet/telnetd.h"
#endif
#include "lnah/lnah.h"

volatile uint8_t main_evt = 0;

int main(void)
{
    myinit();
    lnah_init();
#ifdef ENABLE_TELNET
    io_recv_str_ptr = telnetd_recv_str;
    io_send_str_ptr = telnetd_send_str;
#endif
#ifdef ENABLE_UART
    io_recv_str_ptr = uart_recv_str;
    io_send_str_ptr = uart_send_str;
#endif
    for (;;)
    {
#ifdef ENABLE_PCL
        pcl_io();
#endif
#ifdef ENABLE_VFD
        vfd_upd();
#endif
#ifdef ENABLE_ETH
        eth_io();
        if(myip_tcp_con_closed())
        {
            if((main_evt == EVT_FWUPG) || (main_evt == EVT_REBOOT))
                break;
        }
#endif
    }
    mydeinit();
    dbg_send_int2("reset", main_evt);

#ifdef ENABLE_FLASH
    if(main_evt == EVT_FWUPG)
    {
        flash_copy10();
    }
#endif
    NVIC_SystemReset();
    return 0;
}

#ifdef ENABLE_VFD
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == VFD_TIMx)
    {
        led_toggle();
        vfd_tim_upd();
    }
}
#endif

