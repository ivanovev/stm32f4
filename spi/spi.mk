
CFLAGS += -DHAL_SPI_MODULE_ENABLED
CFLAGS += -DENABLE_SPI
C_SRCS += $(ROOT_DIR)/spi/spi.c
#C_SRCS += $(ROOT_DIR)/spi/spi_it.c
C_SRCS += $(ROOT_DIR)/spi/spi_msp.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_spi.c

