
CFLAGS += -DHAL_UART_MODULE_ENABLED
CFLAGS += -DHAL_USART_MODULE_ENABLED -DMY_UART
C_SRCS += $(ROOT_DIR)/uart/uart.c
C_SRCS += $(ROOT_DIR)/uart/uart_it.c
C_SRCS += $(ROOT_DIR)/uart/uart_msp.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_uart.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_usart.c

