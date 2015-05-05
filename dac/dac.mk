
CFLAGS += -DHAL_DAC_MODULE_ENABLED
CFLAGS += -DENABLE_DAC

C_SRCS += $(ROOT_DIR)/dac/dac.c
C_SRCS += $(ROOT_DIR)/dac/dac_msp.c

C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_dac.c

