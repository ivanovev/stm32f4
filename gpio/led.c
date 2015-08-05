
#include <main.h>
#include "gpio/led.h"

#pragma message "LED: GPIO" STR(LED_GPIO) " PIN" STR(LED_PIN)

void led_init(void)
{
    LED_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(LED_GPIO, LED_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    led_on();
}

void led_on(void)
{
    HAL_GPIO_WritePin(GPIO(LED_GPIO), PIN(LED_PIN), GPIO_PIN_SET);
}

void led_off(void)
{
    HAL_GPIO_WritePin(GPIO(LED_GPIO), PIN(LED_PIN), GPIO_PIN_RESET);
}

void led_toggle(void)
{
    HAL_GPIO_TogglePin(GPIO(LED_GPIO), PIN(LED_PIN));
}

