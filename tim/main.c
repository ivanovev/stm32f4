
#include "main.h"
#include "gpio/led.h"
#include "tim.h"

extern void SystemClock_Config(void);
extern uint32_t SystemCoreClock;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    led_init();
    tim_init();
    for (;;) {
        HAL_Delay(1000);
        //led_toggle();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
}

