
C_SRCS += $(CLASS_DIR)/usb.c
C_SRCS += $(CLASS_DIR)/usbd_cdc_interface.c
#C_SRCS += $(CLASS_DIR)/usbd_cdc_if_template.c
C_SRCS += $(CLASS_DIR)/usbd_desc.c

C_SRCS += $(USBD_DIR)/Class/CDC/Src/usbd_cdc.c

CFLAGS += -I$(USBD_DIR)/Class/CDC/Inc

include $(ROOT_DIR)/tim/tim.mk
