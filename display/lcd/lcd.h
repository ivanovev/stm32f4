
#ifndef __LCD_H__
#define __LCD_H__

#include <main.h>

#define BTNU_GPIO D
#define BTNU_PIN 9

#define BTND_GPIO D
#define BTND_PIN 8

#define BTNL_GPIO B
#define BTNL_PIN 15

#define BTNR_GPIO B
#define BTNR_PIN 10

#define BTNO_GPIO B
#define BTNO_PIN 14

void        lcd_init(void);
void        lcd_deinit(void);
void        lcd_crlf(void);
void        lcd_cls(void);
void        lcd_home(void);
void        lcd_bl(const char *bl);

void        lcd_bytes(const char *str, uint16_t len);
void        lcd_str(const char *str);

#endif

