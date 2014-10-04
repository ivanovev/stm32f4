
CFLAGS += -DHAL_CRC_MODULE_ENABLED -DMY_FLASH
CFLAGS += -DSTM32F407xx
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_flash.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_flash_ex.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_crc.c
C_SRCS += $(ROOT_DIR)/flash/flash.c
C_SRCS += $(ROOT_DIR)/flash/crc_msp.c

