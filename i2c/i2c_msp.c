
#include <main.h>

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef gpio_init;
    I2C_PINMUX(I2Cn, I2C_SCL_GPIO, I2C_SCL_PIN, I2C_SDA_GPIO, I2C_SDA_PIN, GPIO_AF_I2Cx);
    I2Cx_CLK_ENABLE();
}

