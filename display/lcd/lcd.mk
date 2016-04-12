
CFLAGS += -DENABLE_LCD

ifndef ENABLE_I2C
include $(ROOT_DIR)/i2c/i2c.mk
endif

C_SRCS += $(LCD_DIR)/lcd.c

