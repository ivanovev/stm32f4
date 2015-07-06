
#ifndef __SDIO_H__
#define __SDIO_H__

#define SDIO_CMD_GPIO           D
#define SDIO_CMD_PIN            2
#define SDIO_CK_GPIO            C
#define SDIO_CK_PIN             12
#define SDIO_D0_GPIO            C
#define SDIO_D0_PIN             8
#define SDIO_D1_GPIO            C
#define SDIO_D1_PIN             9
#define SDIO_D2_GPIO            C
#define SDIO_D2_PIN             10
#define SDIO_D3_GPIO            C
#define SDIO_D3_PIN             11
#define SDIO_D4_GPIO            B
#define SDIO_D4_PIN             8
#define SDIO_D5_GPIO            B
#define SDIO_D5_PIN             9
#define SDIO_D6_GPIO            C
#define SDIO_D6_PIN             6
#define SDIO_D7_GPIO            C
#define SDIO_D7_PIN             7

#define SDIO_DMAn               2
#define SDIO_DMA_STREAMn        3
#define SDIO_DMA_CHANNELn       4

#define SDIO_DMAx_CHANNEL       JOIN(DMA_CHANNEL_, SDIO_DMA_CHANNELn)
#define SDIO_DMAx_STREAM        JOIN4(DMA, SDIO_DMAn, _Stream, SDIO_DMA_STREAMn)
#define SDIO_DMAx_CLK_ENABLE    JOIN3(__HAL_RCC_DMA, SDIO_DMAn, _CLK_ENABLE)

void sdio_init(void);
void sdio_write(uint8_t *data, uint32_t sz);

#endif
