
#ifndef __ADC_H__
#define __ADC_H__

#include <main.h>

#if ADCn == 1
#define ADCx                            ADC1
#endif

#define ADCx_CHANNEL                    JOIN(ADC_CHANNEL_, ADC_INn)

#define ADCx_CLK_ENABLE                 JOIN3(__HAL_RCC_ADC, ADCn, _CLK_ENABLE)
#define ADCx_FORCE_RESET                JOIN3(__HAL_RCC_ADC, ADCn, _CLK_FORCE_RESET)
#define ADCx_RELEASE_RESET              JOIN3(__HAL_RCC_ADC, ADCn, _CLK_RELEASE_RESET)

#if ADCn == 1
#if ADC_INn == 3
#define ADCx_GPIO                       A
#define ADCx_PIN                        3
#endif

#if ADC_INn == 6
#define ADCx_GPIO                       A
#define ADCx_PIN                        6
#endif

#if ADC_INn == 8
#define ADCx_GPIO                       B
#define ADCx_PIN                        0
#endif

#if ADC_INn == 9
#define ADCx_GPIO                       B
#define ADCx_PIN                        1
#endif

#if ADC_INn == 10
#define ADCx_GPIO                       C
#define ADCx_PIN                        0
#endif
#endif

#ifdef ADC_DMAn
#define ADC_DMAx_CHANNEL                JOIN(DMA_CHANNEL_, ADC_DMA_CHANNELn)
#define ADC_DMAx_STREAM                 JOIN4(DMA, ADC_DMAn, _Stream, ADC_DMA_STREAMn)
#define ADC_DMAx_CLK_ENABLE             JOIN3(__HAL_RCC_DMA, ADC_DMAn, _CLK_ENABLE)
#define ADC_DMAx_IRQn                   JOIN5(DMA, ADC_DMAn, _Stream, ADC_DMA_STREAMn, _IRQn)
#define ADC_DMAx_IRQHandler             JOIN5(DMA, ADC_DMAn, _Stream, ADC_DMA_STREAMn, _IRQHandler)

#define ADCx_TRIGGER                    JOIN3(ADC_EXTERNALTRIGCONV_T, ADC_TIMn, _TRGO)
#define ADC_TIMx                        JOIN(TIM, ADC_TIMn)
#define ADC_TIMx_CLK_ENABLE             JOIN3(__HAL_RCC_TIM, ADC_TIMn, _CLK_ENABLE)
#define ADC_TIMx_IRQn                   JOIN3(TIM, ADC_TIMn, _IRQn)
#define ADC_TIMx_FORCE_RESET            JOIN3(__HAL_RCC_TIM, ADC_TIMn, _FORCE_RESET)
#define ADC_TIMx_RELEASE_RESET          JOIN3(__HAL_RCC_TIM, ADC_TIMn, _RELEASE_RESET)
#endif

void            adc_init(void);
void            adc_start(void);
void            adc_start_sz(uint32_t sz);
uint16_t        adc_get_data(uint8_t *out, uint16_t sz);
void            adc_stop(void);
uint16_t		adc_read(void);

#endif

