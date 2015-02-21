
#include <main.h>
#include "usb.h"

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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
    usb_tim_cb(htim);
}

