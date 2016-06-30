
C_SRCS += $(ROOT_DIR)/util/system_stm32f4xx.c
C_SRCS += $(ROOT_DIR)/util/system_clock.c
C_SRCS += $(ROOT_DIR)/util/system_it.c
C_SRCS += $(ROOT_DIR)/util/system_msp.c
C_SRCS += $(ROOT_DIR)/util/queue.c
C_SRCS += $(ROOT_DIR)/util/util.c
C_SRCS += $(ROOT_DIR)/util/version.c
C_SRCS += $(ROOT_DIR)/util/heap1.c
#C_SRCS += $(ROOT_DIR)/util/startup_stm32f4xx.c
#C_SRCS += $(ROOT_DIR)/util/syscalls.c

# add startup file to build
#A_SRCS += $(ROOT_DIR)/util/startup_stm32f4xx.s
A_SRCS += $(ROOT_DIR)/util/startup_stm32f407xx.s
