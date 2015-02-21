
#ifndef __USB_H__
#define __USB_H__

#ifdef ENABLE_USB_CDC
#include "usb/cdc/cdc.h"
#endif

#define USB_TIMn                3
#define USB_TIMx                JOIN(TIM, USB_TIMn)
#define USB_TIMx_INTERVAL       5
#define USB_TIMx_CLK_ENABLE     JOIN3(__TIM, USB_TIMn, _CLK_ENABLE)
#define USB_TIMx_IRQn           JOIN3(TIM, USB_TIMn, _IRQn)
#define USB_TIMx_IRQHandler     JOIN3(TIM, USB_TIMn, _IRQHandler)
#define USB_TIMx_FORCE_RESET    JOIN3(__TIM, USB_TIMn, _FORCE_RESET)
#define USB_TIMx_RELEASE_RESET  JOIN3(__TIM, USB_TIMn, _RELEASE_RESET)

void usb_init(void);
void usb_tim_init(void);
void usb_tim_cb(TIM_HandleTypeDef *htim);

#endif

