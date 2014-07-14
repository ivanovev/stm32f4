
#include "main.h"

extern void SystemClock_Config(void);
extern uint32_t SystemCoreClock;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    led_init();
    led_on();
#if 0
    btn_init();
    led_off();
    for (;;) {
        if(btn_status())
        {
            led_toggle();
            HAL_Delay(1000);
        }
    }
#else
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
#endif
}

