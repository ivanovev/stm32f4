
CFLAGS += -DHAL_GPIO_MODULE_ENABLED
CFLAGS += -DHAL_TIM_MODULE_ENABLED
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_tim.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_tim_ex.c
C_SRCS += $(ROOT_DIR)/tim/tim.c
C_SRCS += $(ROOT_DIR)/tim/tim_it.c
C_SRCS += $(ROOT_DIR)/tim/tim_msp.c

