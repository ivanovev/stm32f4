
#include "tim_it.h"

extern TIM_HandleTypeDef TimHandle;

void TIMx_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}

