
CFLAGS += -DHAL_ADC_MODULE_ENABLED
CFLAGS += -DENABLE_ADC

C_SRCS += $(ROOT_DIR)/adc/adc.c
C_SRCS += $(ROOT_DIR)/adc/adc_msp.c

C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_adc.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_adc_ex.c

