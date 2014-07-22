
#include "main.h"

extern void SystemClock_Config(void);
extern uint32_t SystemCoreClock;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    led_init();
    btn_init();
    led_toggle();
    for (;;) {
        HAL_Delay(1000);
#if 0
        if(btn_status())
            led_toggle();
#else
#if 0
        if(btn_status())
            led_on();
        else
            led_off();
#endif
#endif
    }
}

