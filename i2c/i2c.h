
#ifndef __I2C_H__
#define __I2C_H__

#include <main.h>
#ifdef ENABLE_EEPROM
#include "i2c/eeprom/eeprom.h"
#endif

#ifdef BOARD_MY1
#define I2Cn 1
#define I2C_SCL_GPIO B
#define I2C_SDA_GPIO B
#define I2C_SCL_PIN 6
#define I2C_SDA_PIN 7

#define I2Cx JOIN(I2C, I2Cn)
#define I2Cx_CLK_ENABLE JOIN3(__I2C, I2Cn, _CLK_ENABLE)
#define GPIO_AF_I2Cx JOIN(GPIO_AF4_I2C, I2Cn)
#endif

void        i2c_init(void);
void        i2c_send1(void);
uint32_t    i2c_send(uint16_t addr, uint8_t *buf, uint16_t sz);
uint32_t    i2c_send_hex(uint16_t addr, char *buf);

#endif

