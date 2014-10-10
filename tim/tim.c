
#include <main.h>

#define TIMx    JOIN(TIM, TIMn)

#pragma message "TIM: TIM" STR(TIMn)
#pragma message "TIM: TIM_INTERVAL[ms] = " STR(TIMx_INTERVAL)

TIM_HandleTypeDef htim;
extern void Error_Handler(void);

void tim_init(void)
{
    htim.Instance = TIMx;
    htim.Init.Period = (TIMx_INTERVAL*1000) - 1; // TIM3 fires every USBD_CDC_POLLING_INTERVAL ms
    htim.Init.Prescaler = 84-1; // for System clock at 168MHz, TIM3 runs at 1MHz
    htim.Init.ClockDivision = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&htim);
    if(HAL_TIM_Base_Init(&htim) != HAL_OK)
    { 
        Error_Handler();
    }

    if(HAL_TIM_Base_Start_IT(&htim) != HAL_OK)
    { 
        Error_Handler();
    }
}

void tim_deinit(void)
{
    HAL_TIM_Base_DeInit(&htim);
}

