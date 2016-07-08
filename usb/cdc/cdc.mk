
CFLAGS += -DENABLE_USB_CDC

CDC_DIR=$(ROOT_DIR)/usb/cdc

C_SRCS += $(CDC_DIR)/cdc.c
C_SRCS += $(CDC_DIR)/usbd_cdc_interface.c
#C_SRCS += $(CLASS_DIR)/usbd_cdc_if_template.c
C_SRCS += $(CDC_DIR)/usbd_desc.c

C_SRCS += $(USBD_DIR)/Class/CDC/Src/usbd_cdc.c

CFLAGS += -I$(CDC_DIR)
CFLAGS += -I$(USBD_DIR)/Class/CDC/Inc

