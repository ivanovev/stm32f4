
#include <main.h>
#include "eth/eth.h"
#include "eth/mdio.h"
#ifdef ENABLE_TELNET
#include "eth/myip/mytcp.h"
#include "eth/myip/telnet/telnetd.h"
#endif
#include "core_cm4.h"
#include "flash/flash.h"

volatile uint8_t main_evt = 0;

int main(void)
{
    myinit();
    mdio_read(2);
    mdio_read(3);
#ifdef ENABLE_TELNET
    io_recv_str_ptr = telnetd_recv_str;
    io_send_str_ptr = telnetd_send_str;
#endif
    for (;;)
    {
#ifdef ENABLE_PCL
        pcl_io();
#ifdef ENABLE_FLASH
        if(main_evt & EVT_PCLUPD)
        {
            pcl_clear();
            pcl_init();
            main_evt &= ~EVT_PCLUPD;
            continue;
        }
#endif
        if(main_evt & EVT_BTN)
        {
            pcl_exec("btn_cb");
            main_evt &= ~EVT_BTN;
            continue;
        }
#endif
#ifdef ENABLE_DISPLAY
        display_upd();
#endif
        eth_io();
#ifdef ENABLE_TELNET
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

#if 1
#ifdef ENABLE_TIM
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef ENABLE_DISPLAY
    if(htim->Instance == DISP_TIMx)
    {
        //led_toggle();
        display_tim_upd();
    }
#endif
}
#endif
#endif

