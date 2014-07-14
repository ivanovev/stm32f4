
#include "main.h"
#include "tim.h"

#define TIMx    JOIN(TIM, TIMn)

#pragma message "TIM: TIM" STR(TIMn)
#pragma message "TIM: TIM_INTERVAL = " STR(TIMx_INTERVAL)

TIM_HandleTypeDef TimHandle;
extern void Error_Handler(void);

void tim_init(void)
{
    TimHandle.Instance = TIMx;
    TimHandle.Init.Period = (TIMx_INTERVAL*1000) - 1; // TIM3 fires every USBD_CDC_POLLING_INTERVAL ms
    TimHandle.Init.Prescaler = 84-1; // for System clock at 168MHz, TIM3 runs at 1MHz
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&TimHandle);
    if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    { 
        Error_Handler();
    }

    if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
    { 
        Error_Handler();
    }
}

