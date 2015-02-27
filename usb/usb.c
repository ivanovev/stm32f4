
#include <main.h>
#include "tim/tim.h"
#include "usb/usb.h"

#pragma message "USB_TIM: USB_TIM" STR(USB_TIMn)
#pragma message "USB_TIM: USB_TIM_INTERVAL[ms] = " STR(USB_TIMx_INTERVAL)
TIM_HandleTypeDef husbtim;

void usb_init(void)
{
#ifdef ENABLE_USB_CDC
    usb_cdc_init();
#else
#ifdef ENABLE_USB_HID
    usb_hid_init();
#endif
#endif
}

void usb_deinit(void)
{
    USB_TIMx_FORCE_RESET();
    USB_TIMx_RELEASE_RESET();
}

void usb_tim_init(void)
{
    husbtim.Instance = USB_TIMx;
    husbtim.Init.Period = 10*USB_TIMx_INTERVAL - 1;
    husbtim.Init.Prescaler = tim_get_prescaler(USB_TIMx);
    husbtim.Init.ClockDivision = 0;
    husbtim.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&husbtim) != HAL_OK)
    {
        Error_Handler();
    }

    if(HAL_TIM_Base_Start_IT(&husbtim) != HAL_OK)
    {
        Error_Handler();
    }
}

void usb_tim_cb(TIM_HandleTypeDef *phtim)
{
#ifdef ENABLE_USB_CDC
    if(phtim->Instance == USB_TIMx)
    {
        CDC_TIM_PeriodElapsedCallback(phtim);
    }
#endif
}

