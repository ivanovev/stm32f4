
#ifndef __DAC_H__
#define __DAC_H__

#include <main.h>

#define DAC_OUTn            1
#define DAC_DMAn            1
#define DAC_DMA_STREAMn     5
#define DAC_DMA_CHANNELn    7
#define DAC_TIMn            5


#define DACx                            DAC
#define DACx_CHANNEL                    JOIN(DAC_CHANNEL_, DAC_OUTn)
#define DACx_TRIGGER                    JOIN3(DAC_TRIGGER_T, DAC_TIMn, _TRGO)

#define DACx_CLK_ENABLE()               __HAL_RCC_DAC_CLK_ENABLE()
#define DACx_FORCE_RESET()              __HAL_RCC_DAC_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC_RELEASE_RESET()

#define DACx_GPIO                       A

#if DAC_OUTn == 1
#define DACx_PIN                        4
#endif

#if DAC_OUTn == 2
#define DACx_PIN                        5
#endif

#define DAC_DMAx_CHANNEL                JOIN(DMA_CHANNEL_, DAC_DMA_CHANNELn)
#define DAC_DMAx_STREAM                 JOIN4(DMA, DAC_DMAn, _Stream, DAC_DMA_STREAMn)
#define DAC_DMAx_CLK_ENABLE             JOIN3(__HAL_RCC_DMA, DAC_DMAn, _CLK_ENABLE)

#define DAC_TIMx                        JOIN(TIM, DAC_TIMn)
#define DAC_TIMx_CLK_ENABLE             JOIN3(__HAL_RCC_TIM, DAC_TIMn, _CLK_ENABLE)
#define DAC_TIMx_IRQn                   JOIN3(TIM, DAC_TIMn, _IRQn)
#define DAC_TIMx_FORCE_RESET            JOIN3(__HAL_RCC_TIM, DAC_TIMn, _FORCE_RESET)
#define DAC_TIMx_RELEASE_RESET          JOIN3(__HAL_RCC_TIM, DAC_TIMn, _RELEASE_RESET)

void            dac_init(void);
void            dac_start(const uint8_t *data, uint16_t sz);
void            dac_start_sin(void);
void            dac_stop(void);

#endif
