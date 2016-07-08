
CFLAGS += -DENABLE_DISPLAY

ifndef ENABLE_TIM
include $(ROOT_DIR)/tim/tim.mk
endif

C_SRCS += $(ROOT_DIR)/display/display.c
C_SRCS += $(ROOT_DIR)/display/menu.c

#include $(ROOT_DIR)/display/vfd/vfd.mk
include $(ROOT_DIR)/display/lcd/lcd.mk

