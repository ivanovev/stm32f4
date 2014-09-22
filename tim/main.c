
#include <main.h>

int main(void)
{
    myinit();
    for (;;) {
        HAL_Delay(1000);
        //led_toggle();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
}

