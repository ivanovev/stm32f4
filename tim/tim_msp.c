
#include <main.h>

#define TIMx_CLK_ENABLE     JOIN3(__TIM, TIMn, _CLK_ENABLE)
#define TIMx_IRQn           JOIN3(TIM, TIMn, _IRQn)
#define TIMx_FORCE_RESET JOIN3(__TIM, TIMn, _FORCE_RESET)
#define TIMx_RELEASE_RESET JOIN3(__TIM, TIMn, _RELEASE_RESET)

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    TIMx_CLK_ENABLE();

    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriority(TIMx_IRQn, 4, 0);

    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIMx_IRQn);
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    TIMx_FORCE_RESET();
    TIMx_RELEASE_RESET();
}

