
CFLAGS += -DENABLE_DSP
CMSIS_INC_DIR=$(CMSIS_DIR)/Include
CMSIS_LIB_DIR=$(CMSIS_DIR)/Lib/ARM

LDFLAGS += $(CMSIS_LIB_DIR)/arm_cortexM4l_math.lib

CFLAGS += -I$(CMSIS_INC_DIR)

