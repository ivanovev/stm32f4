
#include <main.h>

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
#ifdef TIMx
    if(htim->Instance == TIMx)
    {
        TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(TIMx_IRQn);
    }
#endif
#ifdef MY_VFD
    if(htim->Instance == VFD_TIMx)
    {
        VFD_TIMx_CLK_ENABLE();
        HAL_NVIC_SetPriority(VFD_TIMx_IRQn, 0xF, 0);
        HAL_NVIC_EnableIRQ(VFD_TIMx_IRQn);
    }
#endif
}

#define TIMx_CH1_GPIO A
#define TIMx_CH1_PIN 5
#define TIMx_AF GPIO_AF1_TIM2
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
#ifdef TIMx
    TIMx_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(TIMx_CH1_GPIO, TIMx_CH1_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_HIGH, TIMx_AF);
#endif
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
#ifdef TIMx
    if(htim->Instance == TIMx)
    {
        TIMx_FORCE_RESET();
        TIMx_RELEASE_RESET();
    }
#endif
#ifdef MY_VFD
    if(htim->Instance == VFD_TIMx)
    {
        VFD_TIMx_FORCE_RESET();
        VFD_TIMx_RELEASE_RESET();
    }
#endif
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef TIMx
    if(htim->Instance == TIMx)
    {
#ifdef MY_ETH
        eth_io();
#else
        led_toggle();
#endif
    }
#endif
#ifdef MY_VFD
    if(htim->Instance == VFD_TIMx)
    {
        led_toggle();
        vfd_tim_upd();
    }
#endif
}

