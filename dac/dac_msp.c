
#include "dac/dac.h"

#ifdef DACx

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
    GPIO_InitTypeDef gpio_init;
    static DMA_HandleTypeDef  hdma_dac;
    GPIO_INIT(DACx_GPIO, DACx_PIN, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
    DACx_CLK_ENABLE();
    DAC_DMAx_CLK_ENABLE();
    DAC_TIMx_CLK_ENABLE();

    hdma_dac.Instance = DAC_DMAx_STREAM;

    hdma_dac.Init.Channel  = DAC_DMAx_CHANNEL;

    hdma_dac.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_dac.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_dac.Init.Mode = DMA_CIRCULAR;
    hdma_dac.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_dac);

#if DAC_OUTn == 1
    __HAL_LINKDMA(hdac, DMA_Handle1, hdma_dac);
#endif

#if DAC_OUTn == 2
    __HAL_LINKDMA(hdac, DMA_Handle2, hdma_dac);
#endif

    //HAL_NVIC_SetPriority(DAC_DMAx_IRQn, 2, 0);
    //HAL_NVIC_EnableIRQ(DAC_DMAx_IRQn);
}

#endif
