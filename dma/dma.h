
#ifndef __DMA_H__
#define __DMA_H__

#include <main.h>

#define DMAn            2
#define DMA_STREAMn     4
#define DMA_CHANNELn    4

#define DMAx_CHANNEL                JOIN(DMA_CHANNEL_, DMA_CHANNELn)
#define DMAx_STREAM                 JOIN4(DMA, DMAn, _Stream, DMA_STREAMn)
#define DMAx_CLK_ENABLE             JOIN3(__HAL_RCC_DMA, DMAn, _CLK_ENABLE)
//#define DMAx_IRQn                   JOIN5(DMA, DMAn, _Stream, DMA_STREAMn, _IRQn)
//#define DMAx_IRQHandler             JOIN5(DMA, DMAn, _Stream, DMA_STREAMn, _IRQHandler)

void            dma_init(void);
void*           dma_memcpy(void* dest, const void* src, uint32_t count);

#endif

