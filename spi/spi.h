
#ifndef __SPI_H__
#define __SPI_H__

#define SPIn 2

#define SPIx JOIN(SPI, SPIn)
#define SPIx_CLK_ENABLE JOIN3(__SPI, SPIn, _CLK_ENABLE)
#define SPIx_IRQn JOIN3(SPI, SPIn, _IRQn)
#define SPIx_IRQHandler JOIN3(SPI, SPIn, _IRQHandler)

#define SPI_CLK_GPIO    B
#define SPI_CLK_PIN     10

#define SPI_MOSI_GPIO   B
#define SPI_MOSI_PIN    15

#define SPI_MISO_GPIO   B
#define SPI_MISO_PIN    14

#if SPIn == 1 || SPIn == 2
#define GPIO_AF_SPIx JOIN(GPIO_AF5_SPI, SPIn)
#endif
#if SPIn == 3
#define GPIO_AF_SPIx JOIN(GPIO_AF6_USART, SPIn)
#endif

#include <main.h>

void                spi_init(void);
volatile uint32_t*  spi_get_reg_ptr(uint8_t nspi, const char *reg);
uint32_t            spi_get_reg(uint8_t nspi, const char *reg);
uint32_t            spi_set_reg(uint8_t nspi, const char *reg, uint32_t val);
uint32_t            spi_send(uint8_t nspi, GPIO_TypeDef *csgpiox, uint8_t csgpion, uint8_t *txd, int len);

#endif

