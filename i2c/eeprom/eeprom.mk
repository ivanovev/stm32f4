
ifndef ENABLE_EEPROM
CFLAGS += -DENABLE_EEPROM
ENABLE_EEPROM = 1

EEPROM_DIR = $(ROOT_DIR)/i2c/eeprom
CFLAGS += -I$(EEPROM_DIR)

C_SRCS += $(EEPROM_DIR)/eeprom.c

include $(ROOT_DIR)/i2c/eeprom/24lc16/24lc16.mk
#include $(ROOT_DIR)/i2c/eeprom/25lc64/25lc64.mk
endif

