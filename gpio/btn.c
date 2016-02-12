
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

void btn_irq_init(GPIO_TypeDef *gpiox, uint32_t pin)
{
    uint32_t irqn;
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = (1 << pin);
    gpio_init.Mode = GPIO_MODE_IT_FALLING;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_LOW;
    gpio_init.Alternate = 0;
    HAL_GPIO_Init(gpiox, &gpio_init);
    if(pin == 0)
        irqn = EXTI0_IRQn;
    else if(pin == 1)
        irqn = EXTI1_IRQn;
    else if(pin == 2)
        irqn = EXTI2_IRQn;
    else if(pin == 3)
        irqn = EXTI3_IRQn;
    else if(pin == 4)
        irqn = EXTI4_IRQn;
    else if((5 <= pin) && (pin <= 9))
        irqn = EXTI9_5_IRQn;
    else if((10 <= pin) && (pin <= 15))
        irqn = EXTI15_10_IRQn;
    HAL_NVIC_SetPriority(irqn, 0xF, 0);
    HAL_NVIC_EnableIRQ(irqn);

}

