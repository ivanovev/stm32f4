
#CFLAGS += -DHAL_DMA_MODULE_ENABLED
CFLAGS += -DENABLE_DMA

#C_SRCS += $(ROOT_DIR)/dma/dma.c

C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_dma.c

