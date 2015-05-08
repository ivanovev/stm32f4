
#include "dma_it.h"

#ifdef DMAx_IRQHandler
extern DMA_HandleTypeDef hdma;

void DMAx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma);
}
#endif

