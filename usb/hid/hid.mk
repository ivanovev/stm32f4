
CFLAGS += -DENABLE_USB_HID
HID_DIR = $(ROOT_DIR)/usb/hid

C_SRCS += $(HID_DIR)/hid.c
C_SRCS += $(HID_DIR)/usbd_desc.c

C_SRCS += $(USBD_DIR)/Class/HID/Src/usbd_hid.c

CFLAGS += -I$(HID_DIR)
CFLAGS += -I$(USBD_DIR)/Class/HID/Inc

