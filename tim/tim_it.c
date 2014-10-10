
#include "tim_it.h"

extern TIM_HandleTypeDef htim;

void TIMx_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim);
}

