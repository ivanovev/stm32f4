
CFLAGS += -DHAL_SD_MODULE_ENABLED -DENABLE_SDIO
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_ll_sdmmc.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_sd.c
C_SRCS += $(ROOT_DIR)/sdio/sdio.c
C_SRCS += $(ROOT_DIR)/sdio/sdio_it.c
C_SRCS += $(ROOT_DIR)/sdio/sdio_msp.c
C_SRCS += $(ROOT_DIR)/sdio/pcl_sdio.c

