
#include <main.h>

int main(void)
{
    myinit();
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
}
#endif

