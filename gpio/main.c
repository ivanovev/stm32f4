
#include <main.h>

int main(void)
{
    myinit();
    for (;;) {
        HAL_Delay(1);
        led_off();
        HAL_Delay(19);
        led_on();
    }
}

