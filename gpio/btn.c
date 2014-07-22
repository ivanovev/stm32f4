
#include "main.h"

#ifdef BOARD_E407
#pragma message "BTN: GPIO" STR(BTN_GPIO) " PIN" STR(BTN_PIN)
#endif

void btn_init(void)
{
#ifdef BOARD_E407
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(BTN_GPIO, BTN_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
#endif
}

uint8_t btn_status(void)
{
#ifdef BOARD_E407
    return HAL_GPIO_ReadPin(GPIO(BTN_GPIO), PIN(BTN_PIN));
#else
    return 0;
#endif
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if(pin == PIN(BTN_PIN))
    {
        led_toggle();
    }
}

