
#include <main.h>

int main(void)
{
    myinit();
    //mco_init();
    //HAL_Delay(1000);
    led_on();
    //btn_init();
    //led_toggle();
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
#if 0
        if(btn_state())
            led_toggle();
#else
#if 0
        if(btn_state())
            led_on();
        else
            led_off();
#endif
#endif
    }
}

