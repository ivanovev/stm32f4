
CFLAGS += -DHAL_PCD_MODULE_ENABLED
CFLAGS += -DHAL_TIM_MODULE_ENABLED
CFLAGS += -DUSE_USB_FS
CFLAGS += -DMY_USB
CFLAGS += -I$(ROOT_DIR)/usb
CFLAGS += -I$(USBD_DIR)/Core/Inc

CLASS_DIR = $(ROOT_DIR)/usb/cdc
CFLAGS += -I$(CLASS_DIR)

C_SRCS += $(ROOT_DIR)/usb/usb_it.c
C_SRCS += $(ROOT_DIR)/usb/usbd_conf.c
C_SRCS += $(STM_HAL)/Src/stm32f4xx_hal_pcd.c
C_SRCS += $(STM_HAL)/Src/stm32f4xx_hal_pcd_ex.c
C_SRCS += $(STM_HAL)/Src/stm32f4xx_ll_usb.c
C_SRCS += $(USBD_DIR)/Core/Src/usbd_core.c
C_SRCS += $(USBD_DIR)/Core/Src/usbd_ioreq.c
C_SRCS += $(USBD_DIR)/Core/Src/usbd_ctlreq.c

include $(CLASS_DIR)/$(shell basename $(CLASS_DIR)).mk

