
#ifndef __DAC_H__
#define __DAC_H__

#include <main.h>

#define DACx                            DAC
#define DACx_CHANNEL                    JOIN(DAC_CHANNEL_, DAC_OUTn)
#define DACx_TRIGGER                    JOIN3(DAC_TRIGGER_T, DAC_TIMn, _TRGO)

#define DACx_CLK_ENABLE()               __HAL_RCC_DAC_CLK_ENABLE()
#define DACx_FORCE_RESET()              __HAL_RCC_DAC_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC_RELEASE_RESET()

#define DACx_GPIO                       A

#if DAC_OUTn == 1
#pragma message "DAC_OUT: DAC" STR(DAC_OUTn)
#define DACx_PIN                        4
#endif

#if DAC_OUTn == 2
#define DACx_GPIO                       A
#define DACx_PIN                        5
#endif

#define DACx_DMA_CHANNEL                JOIN(DMA_CHANNEL_, DAC_DMA_CHANNELn)
#define DACx_DMA_STREAM                 JOIN4(DMA, DAC_DMAn, _Stream, DAC_DMA_STREAMn)
#define DACx_DMA_CLK_ENABLE             JOIN3(__HAL_RCC_DMA, DAC_DMAn, _CLK_ENABLE)

#define DACx_TIM                        JOIN(TIM, DAC_TIMn)
#define DACx_TIM_CLK_ENABLE             JOIN3(__HAL_RCC_TIM, DAC_TIMn, _CLK_ENABLE)
#define DACx_TIM_IRQn                   JOIN3(TIM, DAC_TIMn, _IRQn)
#define DACx_TIM_FORCE_RESET            JOIN3(__HAL_RCC_TIM, DAC_TIMn, _FORCE_RESET)
#define DACx_TIM_RELEASE_RESET          JOIN3(__HAL_RCC_TIM, DAC_TIMn, _RELEASE_RESET)

void            dac_init(void);
uint16_t        dac_start(uint16_t *data, uint16_t sz);
void            dac_stop(void);

#endif
