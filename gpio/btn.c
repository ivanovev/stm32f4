
#include <main.h>

#ifdef BTN_PIN
#pragma message "BTN: GPIO" STR(BTN_GPIO) " PIN" STR(BTN_PIN)

void btn_init(void)
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(BTN_GPIO, BTN_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

uint16_t btn_state(void)
{
    return HAL_GPIO_ReadPin(GPIO(BTN_GPIO), PIN(BTN_PIN));
}

#endif

