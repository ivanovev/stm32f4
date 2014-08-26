
#include "main.h"

#ifdef BTN_PIN
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PIN(BTN_PIN));
}
#endif

