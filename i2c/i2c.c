
#include "i2c/i2c.h"
#include "util/queue.h"

#pragma message "I2C: I2C" STR(I2Cn)

I2C_HandleTypeDef hi2c;

void i2c_init(void)
{
    hi2c.Instance             = I2Cx;

    hi2c.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c.Init.ClockSpeed      = 100000;
    hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
    hi2c.Init.OwnAddress1     = 0xAA;
    hi2c.Init.OwnAddress2     = 0x55;

    if(HAL_I2C_Init(&hi2c) != HAL_OK)
    {
        /* Initialization Error */
        //Error_Handler();
        led_on();
    }
}

void i2c_send1(void)
{
    char buf[] = {0x00, 0x00};
    if(HAL_I2C_Master_Transmit(&hi2c, (uint16_t)0xAE, (uint8_t*)buf, 2, 10000) != HAL_OK)
    {
        if (HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF)
        {
            led_on();
            return;
        }
    }
    while(HAL_I2C_Master_Receive(&hi2c, (uint16_t)0xAF, (uint8_t *)buf, 2, 10000) != HAL_OK)
    {
        if (HAL_I2C_GetError(&hi2c) != HAL_I2C_ERROR_AF)
        {
            led_on();
            return;
        }
    }
}

