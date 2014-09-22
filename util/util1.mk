
#BOARD='HYSTM32'
BOARD='MY1'
ifeq ($(BOARD), 'HYSTM32')
    CFLAGS += -DBOARD_HYSTM32
endif
ifeq ($(BOARD), 'E407')
    CFLAGS += -DBOARD_E407
endif
ifeq ($(BOARD), 'MY1')
    CFLAGS += -DBOARD_MY1
endif
#CFLAGS += -DSTM32F407xx

CUBE_DIR=/home/$(USER)/src/STM32Cube_FW_F4_V1.3.0
HAL_DIR=$(CUBE_DIR)/Drivers/STM32F4xx_HAL_Driver
CMSIS_DIR=$(CUBE_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx
USBD_DIR=$(CUBE_DIR)/Middlewares/ST/STM32_USB_Device_Library

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PWD = $(shell pwd)
PROJ = $(shell basename $(PWD))

ROOT_DIR = ..
UTIL_DIR = $(ROOT_DIR)/util

