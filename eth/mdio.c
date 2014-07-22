
#include "eth/mdio.h"

extern ETH_HandleTypeDef heth;

uint32_t mdio_read(uint16_t reg)
{
    uint32_t val = 0;
    HAL_ETH_ReadPHYRegister(&heth, reg, &val);
    return val;
}

uint32_t mdio_write(uint16_t reg, uint16_t val)
{
    return HAL_ETH_WritePHYRegister(&heth, reg, val);
}

