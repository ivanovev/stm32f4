
CFLAGS += -DHAL_RNG_MODULE_ENABLED -DENABLE_RNG
C_SRCS += $(HAL_DIR)/Src/stm32f4xx_hal_rng.c
C_SRCS += $(ROOT_DIR)/rng/rng.c
C_SRCS += $(ROOT_DIR)/rng/rng_msp.c

