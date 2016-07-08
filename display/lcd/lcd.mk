
CFLAGS += -DENABLE_LCD

ifndef ENABLE_I2C
include $(ROOT_DIR)/i2c/i2c.mk
endif

C_SRCS += $(ROOT_DIR)/display/lcd/lcd.c

