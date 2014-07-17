
#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.h"

volatile uint32_t* gpio_get_reg_ptr(GPIO_TypeDef *gpiox, const char *reg);
uint32_t    gpio_get_reg(GPIO_TypeDef *gpiox, const char *reg);
uint32_t    gpio_set_reg(GPIO_TypeDef *gpiox, const char *reg, uint32_t val);

#endif

