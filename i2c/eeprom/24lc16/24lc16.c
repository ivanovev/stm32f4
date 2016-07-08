
#include "i2c/eeprom/eeprom.h"

extern I2C_HandleTypeDef hi2c;
extern volatile int g_eeprom_write_enable;

uint16_t eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    if(g_eeprom_write_enable == 0)
        return 0;
    uint8_t buf[34];
    uint16_t a = EEPROM_WRITE_ADDR & 0xF0;
    if(addr > 0x7FF)
        return 0;
    if(sz > 32)
        sz = 32;
    a += (addr >> 8) << 1;
    buf[0] = (addr & 0xFF);
    mymemcpy(&(buf[1]), data, sz);
    if(HAL_I2C_Master_Transmit(&hi2c, (uint16_t)a, (uint8_t*)buf, sz + 1, 10000) != HAL_OK)
    {
        //if(HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF)
        {
            led_on();
            return 0;
        }
    }
    return sz;
}

uint16_t eeprom_read_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    uint8_t buf[34];
    uint16_t a = EEPROM_WRITE_ADDR & 0xF0;
    if(addr > 0x7FF)
        return 0;
    if(sz > 32)
        sz = 32;
    a += (addr >> 8) << 1;
    buf[0] = (addr & 0xFF);
    if(HAL_I2C_Master_Transmit(&hi2c, (uint16_t)a, (uint8_t*)buf, 1, 10000) != HAL_OK)
    {
        //if(HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF)
        {
            led_on();
            return 0;
        }
    }
    a += 1;
    if(HAL_I2C_Master_Receive(&hi2c, (uint16_t)a, (uint8_t *)buf, sz, 10000) != HAL_OK)
    {
        //if(HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF)
        {
            led_on();
            return 0;
        }
    }
    mymemcpy(data, buf, sz);
    return sz;
}

