
#BOARD='HYSTM32'
BOARD='MY1'
#BOARD='E407'
ifeq ($(BOARD), 'HYSTM32')
    CFLAGS += -DBOARD_HYSTM32
endif
ifeq ($(BOARD), 'E407')
    CFLAGS += -DBOARD_E407
endif
ifeq ($(BOARD), 'MY1')
    CFLAGS += -DBOARD_MY1
endif

CUBE_DIR=/home/$(USER)/src/STM32Cube_FW_F4_V1.11.0
HAL_DIR=$(CUBE_DIR)/Drivers/STM32F4xx_HAL_Driver
CMSIS_DIR=$(CUBE_DIR)/Drivers/CMSIS
USBD_DIR=$(CUBE_DIR)/Middlewares/ST/STM32_USB_Device_Library

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PWD = $(shell pwd)
PROJ = $(shell basename $(PWD))

ROOT_DIR = ..
UTIL_DIR = $(ROOT_DIR)/util

LDSCRIPT_RAM = $(UTIL_DIR)/stm32f4_ram.ld
LDSCRIPT_FLASH = $(UTIL_DIR)/stm32f4_flash.ld
#LDSCRIPT = $(LDSCRIPT_RAM)
LDSCRIPT = $(LDSCRIPT_FLASH)
ifeq ($(LDSCRIPT), $(LDSCRIPT_RAM))
    CFLAGS += -DVECT_TAB_SRAM
endif

CFLAGS += -O0 -ggdb -Wall
#CFLAGS += -O1 -Wall
#CFLAGS += -Os -Wall

