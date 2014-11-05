
#include "i2c/eeprom.h"

extern I2C_HandleTypeDef hi2c;

#define EEPROM_WRITE_ADDR 0xAE
#define EEPROM_READ_ADDR 0xAF

uint16_t eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    uint8_t buf[34];
    if(sz > 32)
        sz = 32;
    buf[0] = (addr >> 8);
    buf[1] = (addr & 0xFF);
    mymemcpy(&(buf[2]), data, sz);
    if(HAL_I2C_Master_Transmit(&hi2c, (uint16_t)EEPROM_WRITE_ADDR, (uint8_t*)buf, sz + 2, 10000) != HAL_OK)
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
    if(sz > 32)
        sz = 32;
    buf[0] = (addr >> 8);
    buf[1] = (addr & 0xFF);
    if(HAL_I2C_Master_Transmit(&hi2c, (uint16_t)EEPROM_WRITE_ADDR, (uint8_t*)buf, 2, 10000) != HAL_OK)
    {
        //if(HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF)
        {
            led_on();
            return 0;
        }
    }
    if(HAL_I2C_Master_Receive(&hi2c, (uint16_t)EEPROM_READ_ADDR, (uint8_t *)buf, sz, 10000) != HAL_OK)
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

#define EEPROM_IPADDR_PAGE 0x1FFC

uint32_t eeprom_ipaddr_read(void)
{
    uint8_t buf[5];
    buf[0] = 0xFF;
    buf[1] = 0xFF;
    buf[2] = 0xFF;
    buf[3] = 0xFF;
    eeprom_read_data(EEPROM_IPADDR_PAGE, buf, 4);
    if((buf[0] == 0xFF) || (buf[1] == 0xFF) || (buf[2] == 0xFF) || (buf[3] == 0xFF))
        return LOCAL_IP_ADDR;
    uint32_t ipaddr = *(uint32_t*)&buf;
    return ipaddr;
}

void eeprom_ipaddr_write(uint32_t ipaddr)
{
    uint8_t *data = (uint8_t*)&ipaddr;
    eeprom_write_data(EEPROM_IPADDR_PAGE, data, 4);
}

