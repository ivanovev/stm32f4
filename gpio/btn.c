
#include "main.h"

#ifdef BOARD_E407
#pragma message "BTN: GPIO" STR(BTN_GPIO) " PIN" STR(BTN_PIN)
#endif

void btn_init(void) {
#ifdef BOARD_E407
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(BTN_GPIO, BTN_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
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

