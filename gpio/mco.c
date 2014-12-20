
#include <main.h>

#ifdef MCO_PIN
#pragma message "MCO: GPIO" STR(MCO_GPIO) " PIN" STR(MCO_PIN)

void mco_init(void)
{
    GPIO_InitTypeDef gpio_init;
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
    GPIO_INIT(MCO_GPIO, MCO_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF0_MCO);
}

#endif

