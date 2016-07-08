
CFLAGS += -DHAL_PCD_MODULE_ENABLED
CFLAGS += -DUSE_USB_FS
CFLAGS += -DENABLE_USB
CFLAGS += -I$(USBD_DIR)/Core/Inc

C_SRCS += $(ROOT_DIR)/usb/usb.c
C_SRCS += $(ROOT_DIR)/usb/usb_it.c
C_SRCS += $(STM_HAL)/Src/stm32f4xx_hal_pcd.c
C_SRCS += $(STM_HAL)/Src/stm32f4xx_hal_pcd_ex.c
C_SRCS += $(STM_HAL)/Src/stm32f4xx_ll_usb.c
C_SRCS += $(USBD_DIR)/Core/Src/usbd_core.c
C_SRCS += $(USBD_DIR)/Core/Src/usbd_ioreq.c
C_SRCS += $(USBD_DIR)/Core/Src/usbd_ctlreq.c

include $(ROOT_DIR)/usb/cdc/cdc.mk
#include $(ROOT_DIR)/usb/hid/hid.mk
include $(ROOT_DIR)/usb/usbd_conf/usbd_conf.mk

