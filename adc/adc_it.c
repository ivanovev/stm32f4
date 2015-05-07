
#include "adc_it.h"

#ifdef ADCn
extern ADC_HandleTypeDef hadc;

void ADC_DMAx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(hadc.DMA_Handle);
}
#endif

