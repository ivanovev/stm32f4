
#include <main.h>
#include "gpio/led.h"

q15_t src[DSP_BLOCK_SIZE], dst[DSP_BLOCK_SIZE];

int main(void)
{
    myinit();
    uint16_t i;
    for(i = 0; i < DSP_BLOCK_SIZE; i++)
    {
        src[i] = i;
        dst[i] = 0;
    }
    dsp_io((uint8_t*)src, 2*DSP_BLOCK_SIZE, dst);
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

