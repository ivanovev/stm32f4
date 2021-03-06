
#include "spi/spi.h"
#include "util/util.h"

#ifndef SPI_TIMEOUT_MAX
#define SPI_TIMEOUT_MAX           0x1000
#endif

#ifdef SPIn
#pragma message "SPI: SPI" STR(SPIn)
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
    phspi->Init.NSS                 = SPI_NSS_HARD_OUTPUT;
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
    if(csgpiox)
    {
        if(((csgpiox->MODER >> 2*csgpion) & 0x3) != 1)
        {
            GPIO_InitTypeDef gpio_init;
            gpio_init.Pin = 1 << csgpion;
            gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
            gpio_init.Pull = GPIO_PULLUP;
            gpio_init.Speed = GPIO_SPEED_LOW;
            HAL_GPIO_WritePin(csgpiox, 1 << csgpion, GPIO_PIN_SET);
            HAL_GPIO_Init(csgpiox, &gpio_init);
            HAL_Delay(1);
        }
    }
    SPI_HandleTypeDef hspi;
    spi_get_handle(&hspi, SPIn, HAL_SPI_STATE_READY);
    if(csgpiox)
        HAL_GPIO_WritePin(csgpiox, 1 << csgpion, GPIO_PIN_RESET);
    uint8_t buf[MAXSTR];
    mymemset(buf, 0, sizeof(buf));
    HAL_Delay(1);
    if(HAL_SPI_TransmitReceive(&hspi, txd, buf, len, SPI_TIMEOUT_MAX) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_Delay(1);
    uint32_t ret = spi_get_reg(nspi, "sr");
    //while(__HAL_SPI_GET_FLAG(&hspi, SPI_FLAG_BSY) != RESET);
    if(csgpiox)
        HAL_GPIO_WritePin(csgpiox, 1 << csgpion, GPIO_PIN_SET);
    mymemcpy(txd, buf, len);
    return ret;
}

int32_t spi_cr1_bits(uint8_t nspi, uint8_t start, uint8_t stop, int32_t val)
{
    SPI_TypeDef* pspi = spi_get_instance(nspi);
    if(!pspi)
        return 0;
    int32_t spe = 0;
    if(start == stop)
    {
        if((start == 0) || (start == 1))
        {
            spe = spi_cr1_bits(nspi, 6, 6, -1);
            spi_cr1_bits(nspi, 6, 6, 0);
        }
    }
    volatile uint32_t *ptr = &(pspi->CR1);
    uint32_t mask = ((1 << (stop - start + 1)) - 1) << start;
    uint32_t tmp = 0;
    if(val == -1)
        val = (*ptr & mask) >> start;
    else
    {
        tmp = *ptr & ~mask;
        tmp = tmp | ((val << start) & mask);
        *ptr = tmp;
        val = val & (mask >> stop);
    }
    if(spe && (start == stop))
    {
        if((start == 0) || (start == 1))
            spi_cr1_bits(nspi, 6, 6, spe);
    }
    return val;
}

