
#include <main.h>
#include "display/display.h"

volatile uint8_t main_evt = 0;

int main(void)
{
    myinit();
    //led_on();
    display_cls();
    for (;;)
    {
        HAL_Delay(2000);
        display_str("1");
        display_upd();
    }
}

#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
}
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == DISP_TIMx)
    {
        led_toggle();
        display_tim_upd();
    }
}

