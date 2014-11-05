
CFLAGS += -DHAL_UART_MODULE_ENABLED
CFLAGS += -DHAL_USART_MODULE_ENABLED -DMY_VFD
C_SRCS += $(ROOT_DIR)/vfd/vfd.c
C_SRCS += $(ROOT_DIR)/vfd/vfd_menu.c
C_SRCS += $(ROOT_DIR)/vfd/vfd_msp.c
C_SRCS += $(ROOT_DIR)/vfd/vfd_it.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_uart.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_usart.c

