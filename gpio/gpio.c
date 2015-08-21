
#include "gpio/gpio.h"
#include "util/util.h"

volatile uint32_t* gpio_get_reg_ptr(GPIO_TypeDef *gpiox, const char *reg)
{
    if(!mystrncmp(reg, "moder", 5))
        return &(gpiox->MODER);
    if(!mystrncmp(reg, "otyper", 6))
        return &(gpiox->OTYPER);
    if(!mystrncmp(reg, "ospeedr", 7))
        return &(gpiox->OSPEEDR);
    if(!mystrncmp(reg, "pupdr", 5))
        return &(gpiox->PUPDR);
    if(!mystrncmp(reg, "idr", 3))
        return &(gpiox->IDR);
    if(!mystrncmp(reg, "odr", 3))
        return &(gpiox->ODR);
    if(!mystrncmp(reg, "afrl", 4))
        return &(gpiox->AFR[0]);
    if(!mystrncmp(reg, "afrh", 4))
        return &(gpiox->AFR[1]);
    return 0;
}

uint32_t gpio_get_reg(GPIO_TypeDef *gpiox, const char *reg)
{
    volatile uint32_t *reg_ptr = gpio_get_reg_ptr(gpiox, reg);
    if(reg_ptr)
        return *reg_ptr;
    else
        return -1;
}

uint32_t gpio_set_reg(GPIO_TypeDef *gpiox, const char *reg, uint32_t val)
{
    volatile uint32_t *reg_ptr = gpio_get_reg_ptr(gpiox, reg);
    if(reg_ptr)
    {
        *reg_ptr = val;
        return val;
    }
    else
        return -1;
}

