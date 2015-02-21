
CFLAGS += -DENABLE_USB_HID

C_SRCS += $(CLASS_DIR)/hid.c
C_SRCS += $(CLASS_DIR)/usbd_desc.c

C_SRCS += $(USBD_DIR)/Class/HID/Src/usbd_hid.c

CFLAGS += -I$(USBD_DIR)/Class/HID/Inc

