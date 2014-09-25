
CFLAGS += -DMY_FLASH
CFLAGS += -DSTM32F407xx
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_flash.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_flash_ex.c
C_SRCS += $(ROOT_DIR)/flash/flash.c

