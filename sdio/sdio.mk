
CFLAGS += -DHAL_SD_MODULE_ENABLED -DENABLE_SDIO
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_ll_sdmmc.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_sd.c
C_SRCS += $(ROOT_DIR)/sdio/sdio.c

