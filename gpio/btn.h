
#ifndef __BTN_H__
#define __BTN_H__

#include "stdint.h"

#ifdef BOARD_E407
#define BTN_GPIO A
#define BTN_PIN 0
#endif

void            btn_init(void);
void            btn_irq_init(GPIO_TypeDef *gpiox, uint32_t pin);
uint16_t        btn_state(void);

#endif /* __BTN_H__ */

