
#include <main.h>
#include "display/menu.h"
#include "i2c/i2c.h"

#define LCD_ADDR_DATA   0x10
#define LCD_ADDR_CMD    0x20
#define LCD_ADDR_BL     0x30

void lcd_init(void)
{
    lcd_cls();
    lcd_home();
}

void lcd_deinit(void)
{
}

void lcd_bytes(const char *bytes, uint16_t sz)
{
    if(i2c_send(LCD_ADDR_DATA, bytes, sz))
        led_on();
    HAL_Delay(sz*10);
}

void lcd_cls(void)
{
    i2c_send_hex(LCD_ADDR_CMD, "0x01");
    HAL_Delay(50);
}

void lcd_home(void)
{
    i2c_send_hex(LCD_ADDR_CMD, "0x02");
    HAL_Delay(50);
}

void lcd_crlf(void)
{
    i2c_send_hex(LCD_ADDR_CMD, "0x2C");
    HAL_Delay(50);
    i2c_send_hex(LCD_ADDR_CMD, "0xA8");
    HAL_Delay(50);
}

void lcd_bl(const char *bl)
{
    i2c_send_hex(LCD_ADDR_BL, bl);
    HAL_Delay(50);
}

