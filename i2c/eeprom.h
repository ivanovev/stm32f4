
#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <main.h>

uint16_t eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz);
uint16_t eeprom_read_data(uint16_t addr, uint8_t *data, uint16_t sz);

uint32_t    eeprom_ipaddr_read(void);
void        eeprom_ipaddr_write(uint32_t ipaddr);

#endif

