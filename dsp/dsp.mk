
CFLAGS += -DENABLE_DSP
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
LDFLAGS += -Xlinker --wrap=memset
LDFLAGS += -Xlinker --wrap=memclr

CMSIS_INC_DIR=$(CMSIS_DIR)/Include
CMSIS_SRC_DIR=$(CMSIS_DIR)/DSP_Lib/Source
CMSIS_LIB_DIR=$(CMSIS_DIR)/Lib/ARM

#LDFLAGS += $(CMSIS_LIB_DIR)/arm_cortexM4lf_math.lib
C_SRCS += $(CMSIS_SRC_DIR)/FilteringFunctions/arm_fir_init_f32.c

CFLAGS += -I$(CMSIS_INC_DIR)

