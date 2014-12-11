
CFLAGS += -DHAL_GPIO_MODULE_ENABLED -DENABLE_GPIO
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_gpio.c
C_SRCS += $(ROOT_DIR)/gpio/led.c
C_SRCS += $(ROOT_DIR)/gpio/btn.c
C_SRCS += $(ROOT_DIR)/gpio/gpio.c
C_SRCS += $(ROOT_DIR)/gpio/gpio_it.c
C_SRCS += $(ROOT_DIR)/gpio/gpio_msp.c

