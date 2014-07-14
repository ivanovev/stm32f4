
#ifndef __LED_H__
#define __LED_H__

#ifdef BOARD_HYSTM32
#define LED_GPIO F
#define LED_PIN 6
#endif

#ifdef BOARD_E407
#define LED_GPIO C
#define LED_PIN 13
#endif

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

#endif /* __LED_H__ */
