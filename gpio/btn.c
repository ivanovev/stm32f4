
#ifdef BOARD_OLIMEX

#include "main.h"

#pragma message "BTN: GPIO" STR(BTN_GPIO) " PIN" STR(BTN_PIN)

void btn_init(void) {
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(BTN_GPIO, BTN_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
}

uint8_t btn_status(void)
{
    return HAL_GPIO_ReadPin(GPIO(BTN_GPIO), BTN_PIN);
}

#endif

