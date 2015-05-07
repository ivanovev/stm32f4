
#include <main.h>

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
#ifdef TIMn
    if(htim->Instance == TIMx)
    {
        TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(TIMx_IRQn);
    }
#endif
#ifdef ADC_TIMx
    if(htim->Instance == ADC_TIMx)
    {
        ADC_TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(ADC_TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(ADC_TIMx_IRQn);
    }
#endif
#ifdef DAC_TIMx
    if(htim->Instance == DAC_TIMx)
    {
        DAC_TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(DAC_TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(DAC_TIMx_IRQn);
    }
#endif
#ifdef PTP_TIMx
    if(htim->Instance == PTP_TIMx)
    {
        PTP_TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(PTP_TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(PTP_TIMx_IRQn);
    }
#endif
#ifdef VFD_TIMx
    if(htim->Instance == VFD_TIMx)
    {
        VFD_TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(VFD_TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(VFD_TIMx_IRQn);
    }
#endif
#ifdef USB_TIMx
    if(htim->Instance == USB_TIMx)
    {
        USB_TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(USB_TIMx_IRQn, 0x5, 0);
        HAL_NVIC_EnableIRQ(USB_TIMx_IRQn);
    }
#endif
}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
#ifdef PTP_TIMx
    PTP_TIMx_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(PTP_TIMx_CH1_GPIO, PTP_TIMx_CH1_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_HIGH, PTP_TIMx_AF);
#endif
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
#ifdef TIMn
    if(htim->Instance == TIMx)
    {
        TIMx_FORCE_RESET();
        TIMx_RELEASE_RESET();
    }
#endif
}

