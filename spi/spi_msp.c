
#include <main.h>

#ifdef SPIn
#pragma message "SPI_CLK: GPIO" STR(SPI_CLK_GPIO) " PIN" STR(SPI_CLK_PIN)
#pragma message "SPI_MOSI: GPIO" STR(SPI_MOSI_GPIO) " PIN" STR(SPI_MOSI_PIN)
#pragma message "SPI_MISO: GPIO" STR(SPI_MISO_GPIO) " PIN" STR(SPI_MISO_PIN)

#pragma message "SPI_AF: " STR(GPIO_AF_SPIx)
#pragma message "SPIx_IRQn: " STR(SPIx_IRQn)
#endif

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init;
#ifdef SPIn
    if(hspi->Instance == SPIx)
    {
        SPI_PINMUX(SPI_CLK_GPIO, SPI_CLK_PIN, SPI_MOSI_GPIO, SPI_MOSI_PIN, SPI_MISO_GPIO, SPI_MISO_PIN, GPIO_AF_SPIx);
        SPIx_CLK_ENABLE();

        //HAL_NVIC_SetPriority(SPIx_IRQn, 0, 1);
        //HAL_NVIC_EnableIRQ(SPIx_IRQn);
    }
#endif

}
