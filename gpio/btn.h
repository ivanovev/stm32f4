
#ifndef __BTN_H__
#define __BTN_H__

#include "stdint.h"

#ifdef BOARD_E407
#define BTN_GPIO A
#define BTN_PIN 0

void btn_init(void);
uint8_t btn_status(void);

#endif

#endif /* __BTN_H__ */

