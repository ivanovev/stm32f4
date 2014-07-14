
#include "usb.h"
#include "usbd_hid.h"
#include "uart/uart.h"
#include "stm32f4xx_hal.h"

USBD_HandleTypeDef  USBD_Device;

void usb_init(void)
{
#ifdef HAL_UART_MODULE_ENABLED
    uart_send_str("usbd_init", 1);
#endif
    USBD_Init(&USBD_Device, &HID_Desc, 0);

    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, &USBD_HID);

    /* Start Device Process */
    USBD_Start(&USBD_Device);
}

