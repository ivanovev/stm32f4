
#include "sdio/sdio_it.h"

extern SD_HandleTypeDef hsd;

void SDIO_DMAx_IRQHandler(void)
{
    //dbg_send_hex2("lisr", DMA2->LISR);
    HAL_DMA_IRQHandler(hsd.hdmarx);
}

void SDIO_IRQHandler(void)
{
    HAL_SD_IRQHandler(&hsd);
}

