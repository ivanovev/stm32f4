
CFLAGS += -DENABLE_VFD

ifndef ENABLE_UART
include $(ROOT_DIR)/uart/uart.mk
endif

C_SRCS += $(ROOT_DIR)/display/vfd/vfd.c

