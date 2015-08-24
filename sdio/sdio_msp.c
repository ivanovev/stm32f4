
#include <main.h>

void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(SDIO_CMD_GPIO, SDIO_CMD_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_CK_GPIO, SDIO_CK_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D0_GPIO, SDIO_D0_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D1_GPIO, SDIO_D1_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D2_GPIO, SDIO_D2_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D3_GPIO, SDIO_D3_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D4_GPIO, SDIO_D4_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D5_GPIO, SDIO_D5_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D6_GPIO, SDIO_D6_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);
    GPIO_INIT(SDIO_D7_GPIO, SDIO_D7_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF12_SDIO);

    __HAL_RCC_SDIO_CLK_ENABLE();
    //__HAL_RCC_DMA2_CLK_ENABLE();
    SDIO_DMAx_CLK_ENABLE();

    static DMA_HandleTypeDef  hdma_sdio;
    hdma_sdio.Instance = SDIO_DMAx_STREAM;

    hdma_sdio.Init.Channel  = SDIO_DMAx_CHANNEL;
    hdma_sdio.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio.Init.Mode = DMA_PFCTRL;
    //hdma_sdio.Init.Mode = DMA_NORMAL;
    hdma_sdio.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_sdio.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    //hdma_sdio.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_sdio.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_sdio.Init.PeriphBurst = DMA_PBURST_SINGLE;

    HAL_DMA_DeInit(&hdma_sdio);
    HAL_DMA_Init(&hdma_sdio);

    __HAL_LINKDMA(hsd, hdmarx, hdma_sdio);

    hsd->hdmarx->XferCpltCallback  = sdio_dma_rxcpltcb;
    hsd->hdmarx->XferErrorCallback = sdio_dma_rxerrorcb;

    HAL_NVIC_SetPriority(SDIO_DMAx_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDIO_DMAx_IRQn);
    __HAL_SD_SDIO_ENABLE_IT(hsd, (SDIO_IT_DATAEND | SDIO_IT_RXOVERR));
}

void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd)
{
}

