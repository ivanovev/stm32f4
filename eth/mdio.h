
#ifndef __MDIO_H__
#define __MDIO_H__

#include "main.h"

uint32_t    mdio_read(uint16_t reg);
uint32_t    mdio_write(uint16_t reg, uint16_t val);

#endif

