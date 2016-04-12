
CFLAGS += -DENABLE_DISPLAY

#VFD_DIR = $(ROOT_DIR)/display/vfd
#include $(VFD_DIR)/vfd.mk

LCD_DIR = $(ROOT_DIR)/display/lcd
include $(LCD_DIR)/lcd.mk

ifndef ENABLE_TIM
include $(ROOT_DIR)/tim/tim.mk
endif

C_SRCS += $(ROOT_DIR)/display/display.c
C_SRCS += $(ROOT_DIR)/display/menu.c

