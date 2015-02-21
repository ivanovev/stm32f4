
#include <main.h>
#include "cptr.h"

#ifdef ENABLE_USB
#include "usb/cdc/cdc.h"
#endif

#ifdef ENABLE_PCL
#include "pcl/pcl.h"
#include "pcl_wifi.h"
#endif

#ifdef ENABLE_GPIO
#include "gpio/led.h"
#endif

#ifdef ENABLE_UART
#include "uart/uart.h"
#endif

volatile uint8_t reset = 0;

int main(void)
{
    myinit();
    cptr_init();
    //char buf[IO_BUF_SZ];
    io_recv_str_ptr = VCP_read;
    io_send_str_ptr = VCP_write;
    for (;;)
    {
#ifdef ENABLE_PCL
        pcl_io();
#else
        io_echo();
#endif
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
#ifdef ENABLE_USB
    usb_tim_cb(htim);
#endif
}

