
CFLAGS += -DENABLE_USB_CDC

USB_CLASS_DIR=$(ROOT_DIR)/usb/$(USB_CLASS)

C_SRCS += $(USB_CLASS_DIR)/cdc.c
C_SRCS += $(USB_CLASS_DIR)/usbd_cdc_interface.c
#C_SRCS += $(CLASS_DIR)/usbd_cdc_if_template.c
C_SRCS += $(USB_CLASS_DIR)/usbd_desc.c

C_SRCS += $(USBD_DIR)/Class/CDC/Src/usbd_cdc.c

CFLAGS += -I$(USBD_DIR)/Class/CDC/Inc

