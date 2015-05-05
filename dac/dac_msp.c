
#include "dac/dac.h"

#ifdef DACx

#pragma message "DAC_OUT: DAC" STR(DAC_OUTn)
#pragma message "DAC_DMA: DMA" STR(DAC_DMAn)
#pragma message "DAC_TIM: TIM" STR(DAC_TIMn)

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
    GPIO_InitTypeDef gpio_init;
    static DMA_HandleTypeDef  hdma_dac;
    GPIO_INIT(DACx_GPIO, DACx_PIN, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
    DACx_CLK_ENABLE();
    DACx_DMA_CLK_ENABLE();
    DACx_TIM_CLK_ENABLE();

    hdma_dac.Instance = DACx_DMA_STREAM;

    hdma_dac.Init.Channel  = DACx_DMA_CHANNEL;

    hdma_dac.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_dac.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_dac.Init.Mode = DMA_CIRCULAR;
    hdma_dac.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_dac);

    __HAL_LINKDMA(hdac, DMA_Handle1, hdma_dac);

    //HAL_NVIC_SetPriority(DACx_DMA_IRQn, 2, 0);
    //HAL_NVIC_EnableIRQ(DACx_DMA_IRQn);
}

#endif
