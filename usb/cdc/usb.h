
#ifndef __USB_H__
#define __USB_H__

#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"

#define USB_GPIO A
#define USB_VBUS_PIN 9
#define USB_ID_PIN 10
#define USB_DM_PIN 11
#define USB_DP_PIN 12

#ifdef BOARD_OLIMEX
#define USB_VBUSON_GPIO B
#define USB_VBUSON_PIN 0
#endif

void usb_init(void);

#endif

