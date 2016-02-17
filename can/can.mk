
CFLAGS += -DHAL_CAN_MODULE_ENABLED -DENABLE_CAN
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_can.c
C_SRCS += $(ROOT_DIR)/can/can.c
C_SRCS += $(ROOT_DIR)/can/can_it.c
C_SRCS += $(ROOT_DIR)/can/can_msp.c

