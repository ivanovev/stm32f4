
#include "main.h"

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PIN(BTN_PIN));
}

