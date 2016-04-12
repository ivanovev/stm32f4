
#ifndef __LCD_H__
#define __LCD_H__

#include <main.h>

void        lcd_init(void);
void        lcd_deinit(void);
void        lcd_crlf(void);
void        lcd_cls(void);
void        lcd_home(void);

void        lcd_bytes(const char *str, uint16_t len);
void        lcd_str(const char *str);

#endif

