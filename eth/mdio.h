
#ifndef __MDIO_H__
#define __MDIO_H__

#include "main.h"

#define MDC_GPIO C
#define MDC_PIN 1
#define MDIO_GPIO A
#define MDIO_PIN 2

uint32_t    mdio_read(uint16_t reg);
uint32_t    mdio_write(uint16_t reg, uint16_t val);

#endif

