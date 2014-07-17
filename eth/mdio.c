
#include "eth/mdio.h"

#pragma message "PHY_ADDRESS: " STR(PHY_ADDRESS)

extern ETH_HandleTypeDef heth;

uint32_t mdio_read(uint16_t reg)
{
    uint32_t val = 0;
    HAL_ETH_ReadPHYRegister(&heth, reg, &val);
    return val;
}

uint32_t mdio_write(uint16_t reg, uint16_t val)
{
#if 0
    uint32_t tmpreg = 0;     
    __IO uint32_t timeout = 0;
    reg &= 0x1F;

    /* Get the ETHERNET MACMIIAR value */
    tmpreg = ETH->MACMIIAR;
    /* Give the value to the MII data register */
    ETH->MACMIIDR = val;
    /* Write the result value into the MII Address register */
    tmpreg = (PHY_ADDRESS<<11) | (reg<<6) | ETH_MACMIIAR_CR_Div102 | ETH_MACMIIAR_MW | ETH_MACMIIAR_MB;
    ETH->MACMIIAR = tmpreg;
    /* Check for the Busy flag */
    do
    {
        timeout++;
        tmpreg = ETH->MACMIIAR;
    } while ((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_WRITE_TO));
    /* Return ERROR in case of timeout */
    if(timeout == PHY_WRITE_TO)
    {
        return ETH_ERROR;
    }

    /* Return SUCCESS */
    return ETH_SUCCESS;  
#else
    return HAL_ETH_WritePHYRegister(&heth, reg, val);
#endif
}

