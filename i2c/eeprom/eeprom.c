
#include "i2c/eeprom/eeprom.h"

#ifdef ENABLE_24LC16
#include "i2c/eeprom/24lc16/24lc16.h"
#endif
#ifdef ENABLE_25LC64
#include "i2c/eeprom/25lc64/25lc64.h"
#endif

#pragma message "EEPROM_IPADDR_PAGE: " STR(EEPROM_IPADDR_PAGE)
#pragma message "EEPROM_MACADDR_PAGE: " STR(EEPROM_MACADDR_PAGE)

extern I2C_HandleTypeDef hi2c;
volatile int g_eeprom_write_enable = 1;

uint8_t eeprom_write_enable(uint8_t enable)
{
    if((enable == 0) || (enable == 1))
        g_eeprom_write_enable = enable;
    return g_eeprom_write_enable;
}

__weak uint16_t eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    return 0;
}

__weak uint16_t eeprom_read_data(uint16_t addr, uint8_t *data, uint16_t sz)
{
    return 0;
}

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

