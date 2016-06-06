
#include "i2c/eeprom.h"

extern I2C_HandleTypeDef hi2c;
volatile g_eeprom_write_enable = 1;

uint8_t eeprom_write_enable(uint8_t enable)
{
    if((enable == 0) || (enable == 1))
        g_eeprom_write_enable = enable;
    return g_eeprom_write_enable;
}

#define EEPROM_WRITE_ADDR 0xAE
#define EEPROM_READ_ADDR 0xAF

#if 0 // 25LC64
#define EEPROM_IPADDR_PAGE      0x1FFC
#define EEPROM_MACADDR_PAGE     0x1FF6
#define EEPROM_PTP_PORT_ID_PAGE 0x1FF4

uint16_t eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    if(g_eeprom_write_enable == 0)
        return;
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
#else // 24LC16
#define EEPROM_IPADDR_PAGE      0x7FC
#define EEPROM_MACADDR_PAGE     0x7F6
#define EEPROM_PTP_PORT_ID_PAGE 0x7F4

uint16_t eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    if(g_eeprom_write_enable == 0)
        return;
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
#endif

uint32_t eeprom_ipaddr_read(uint8_t *ipaddr)
{
    uint8_t buf[4];
    if(eeprom_read_data(EEPROM_IPADDR_PAGE, buf, 4) == 0)
        return IP_ADDR32;
    if(ipaddr)
        mymemcpy(ipaddr, buf, 4);
    uint32_t ret = *(uint32_t*)&buf;
    ret = HTONS_32(ret);
    dbg_send_hex2("ipaddr", ret);
    return ret;
}

void eeprom_ipaddr_write(uint8_t *ipaddr)
{
    eeprom_write_data(EEPROM_IPADDR_PAGE, ipaddr, 4);
}

void eeprom_macaddr_read(uint8_t *macaddr)
{
    uint8_t buf[6];
    if(eeprom_read_data(EEPROM_MACADDR_PAGE, buf, 6) == 0)
        return;
    mymemcpy(macaddr, buf, 6);
}

void eeprom_macaddr_write(uint8_t *macaddr)
{
    eeprom_write_data(EEPROM_MACADDR_PAGE, macaddr, 6);
}

#ifdef ENABLE_PTP
void eeprom_ptp_port_id_read(uint16_t *port_id)
{
    eeprom_read_data(EEPROM_PTP_PORT_ID_PAGE, (uint8_t*)port_id, 2);
}

void eeprom_ptp_port_id_write(uint8_t *port_id)
{
    eeprom_write_data(EEPROM_PTP_PORT_ID_PAGE, (uint8_t*)port_id, 2);
}
#endif

