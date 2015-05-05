
#include <main.h>
#include "gpio/led.h"

int main(void)
{
    myinit();
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

