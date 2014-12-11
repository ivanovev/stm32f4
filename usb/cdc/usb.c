
#include "usb.h"
#include "tim/tim.h"

USBD_HandleTypeDef  USBD_Device;

void usb_init(void)
{
    /* Init Device Library */
    USBD_Init(&USBD_Device, &VCP_Desc, 0);

    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, &USBD_CDC);

    /* Add CDC Interface Class */
    USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

    /* Start Device Process */
    USBD_Start(&USBD_Device);
    while (!g_VCPInitialized) {}
}

