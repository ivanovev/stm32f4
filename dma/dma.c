
#include <dma/dma.h>
#include "gpio/led.h"

#pragma message "DMA: DMA" STR(DMAn)
#pragma message "DMA_STREAM: DMA_STREAM" STR(DMA_STREAMn)
#pragma message "DMA_CHANNEL: DMA_CHANNEL" STR(DMA_CHANNELn)

DMA_HandleTypeDef  hdma;
void dma_xfer_cplt_cb(DMA_HandleTypeDef * hdma);

void dma_init(void)
{
    DMAx_CLK_ENABLE();

    hdma.Instance = DMAx_STREAM;

    hdma.Init.Channel  = DMAx_CHANNEL;
    hdma.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma.Init.Mode = DMA_NORMAL;
    hdma.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    //hdma.Init.MemBurst = DMA_MBURST_INC16;
    hdma.Init.MemBurst = DMA_MBURST_SINGLE;
    //hdma.Init.PeriphBurst = DMA_PBURST_INC16;
    
    HAL_DMA_Init(&hdma);
    
    //hdma.XferCpltCallback = dma_xfer_cplt_cb;

#ifdef DMAx_IRQn
    HAL_NVIC_SetPriority(DMAx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMAx_IRQn);
#endif
}

void *dma_memcpy(void* dest, const void* src, uint32_t count)
{
#ifdef DMAx_IRQn
    HAL_DMA_Start_IT(&hdma, (uint32_t)src, (uint32_t)dest, count);
#else
    HAL_DMA_Start(&hdma, (uint32_t)src, (uint32_t)dest, count);
#endif

}

void dma_xfer_cplt_cb(DMA_HandleTypeDef * hdma)
{
    led_toggle();
}

