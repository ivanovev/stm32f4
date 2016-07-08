
#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <main.h>

#define EEPROM_WRITE_ADDR 0xAE
#define EEPROM_READ_ADDR 0xAF

uint16_t    eeprom_write_data(uint16_t addr, uint8_t *data, uint16_t sz);
uint16_t    eeprom_read_data(uint16_t addr, uint8_t *data, uint16_t sz);
uint8_t     eeprom_write_enable(uint8_t enable);

#ifdef ENABLE_ETH
uint32_t    eeprom_ipaddr_read(uint8_t *ipaddr);
void        eeprom_ipaddr_write(uint8_t *ipaddr);
void        eeprom_macaddr_read(uint8_t *macaddr);
void        eeprom_macaddr_write(uint8_t *macaddr);
#ifdef ENABLE_PTP
void        eeprom_ptp_port_id_read(uint16_t *port_id);
void        eeprom_ptp_port_id_write(uint8_t *port_id);
#endif
#endif

#endif

