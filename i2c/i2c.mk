
CFLAGS += -DHAL_I2C_MODULE_ENABLED -DMY_I2C
C_SRCS += $(ROOT_DIR)/i2c/i2c.c
C_SRCS += $(ROOT_DIR)/i2c/i2c_msp.c
C_SRCS += $(ROOT_DIR)/i2c/eeprom.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_i2c.c
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_i2c_ex.c

