
#include <main.h>
#include "eth/eth.h"
#include "eth/mdio.h"
#ifdef ENABLE_TELNET
#include "eth/myip/mytcp.h"
#include "eth/myip/telnet/telnetd.h"
#endif
#include "core_cm4.h"
#ifdef ENABLE_FLASH
#include "flash/flash.h"
#endif

volatile uint8_t main_evt = 0;

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

