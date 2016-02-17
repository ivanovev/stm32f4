
#include "adc/adc.h"

#ifdef ADCx

extern void adc_conv_half_cplt_cb(DMA_HandleTypeDef * hdma);
extern void adc_conv_cplt_cb(DMA_HandleTypeDef * hdma);

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef          gpio_init;
    static DMA_HandleTypeDef  hdma_adc;

    GPIO_INIT(ADCx_GPIO, ADCx_PIN, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
    ADCx_CLK_ENABLE();
    ADC_DMAx_CLK_ENABLE();
    ADC_TIMx_CLK_ENABLE();

    hdma_adc.Instance = ADC_DMAx_STREAM;

    hdma_adc.Init.Channel  = ADC_DMAx_CHANNEL;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    //hdma_adc.Init.Mode = DMA_NORMAL;
    hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE;

    HAL_DMA_Init(&hdma_adc);

    __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

    HAL_NVIC_SetPriority(ADC_DMAx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_DMAx_IRQn);
}

#endif

