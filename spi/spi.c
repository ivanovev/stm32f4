
#include "spi/spi.h"
#include "util/util.h"

#ifndef SPI_TIMEOUT_MAX
#define SPI_TIMEOUT_MAX           0x1000
#endif

void spi_get_handle(SPI_HandleTypeDef *phspi, uint8_t nspi, uint8_t state);

void spi_init(void)
{
#ifdef SPIn
    SPI_HandleTypeDef hspi;
    spi_get_handle(&hspi, SPIn, HAL_SPI_STATE_RESET);

    if(HAL_SPI_Init(&hspi) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
#endif
}

SPI_TypeDef* spi_get_instance(uint8_t n)
{
    if(n == 1)
        return SPI1;
    if(n == 2)
        return SPI2;
    if(n == 3)
        return SPI3;
    return 0;
}

void spi_get_handle(SPI_HandleTypeDef *phspi, uint8_t nspi, uint8_t state)
{
    phspi->Instance                 = spi_get_instance(nspi);
    phspi->Init.BaudRatePrescaler   = SPI_BAUDRATEPRESCALER_256;
    phspi->Init.Direction           = SPI_DIRECTION_2LINES;
    phspi->Init.CLKPhase            = SPI_PHASE_1EDGE;
    phspi->Init.CLKPolarity         = SPI_POLARITY_LOW;
    phspi->Init.DataSize            = SPI_DATASIZE_8BIT;
    phspi->Init.FirstBit            = SPI_FIRSTBIT_MSB;
    phspi->Init.TIMode              = SPI_TIMODE_DISABLE;
    phspi->Init.CRCCalculation      = SPI_CRCCALCULATION_DISABLE;
    phspi->Init.CRCPolynomial       = 7;
    phspi->Init.NSS                 = SPI_NSS_SOFT;
    phspi->Init.Mode                = SPI_MODE_MASTER;
    phspi->State                    = state;
}

volatile uint32_t* spi_get_reg_ptr(uint8_t nspi, const char *reg)
{
    SPI_TypeDef *spix = spi_get_instance(nspi);
    if(!nspi)
        return 0;
    if(!mystrncmp(reg, "cr1", 3))
        return &(spix->CR1);
    if(!mystrncmp(reg, "cr2", 3))
        return &(spix->CR2);
    if(!mystrncmp(reg, "sr", 2))
        return &(spix->SR);
    if(!mystrncmp(reg, "dr", 2))
        return &(spix->DR);
    return 0;
}

uint32_t spi_get_reg(uint8_t nspi, const char *reg)
{
    volatile uint32_t *reg_ptr = spi_get_reg_ptr(nspi, reg);
    if(reg_ptr)
        return *reg_ptr;
    else
        return -1;
}

uint32_t spi_set_reg(uint8_t nspi, const char *reg, uint32_t val)
{
    volatile uint32_t *reg_ptr = spi_get_reg_ptr(nspi, reg);
    if(reg_ptr)
    {
        *reg_ptr = val;
        return val;
    }
    else
        return -1;
}

uint32_t spi_send(uint8_t nspi, GPIO_TypeDef *csgpiox, uint8_t csgpion, uint8_t *txd, int len)
{
    SPI_HandleTypeDef hspi;
    spi_get_handle(&hspi, SPIn, HAL_SPI_STATE_READY);
    if(csgpiox)
        HAL_GPIO_WritePin(csgpiox, 1 << csgpion, GPIO_PIN_RESET);
    uint8_t buf[MAXSTR];
    if(HAL_SPI_TransmitReceive(&hspi, txd, buf, len, SPI_TIMEOUT_MAX) != HAL_OK)
    {
        Error_Handler();
    }
    uint32_t ret = spi_get_reg(nspi, "sr");
    //while(__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_BSY) != RESET);
    HAL_Delay(1);
    if(csgpiox)
        HAL_GPIO_WritePin(csgpiox, 1 << csgpion, GPIO_PIN_SET);
    return ret;
}

