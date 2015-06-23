
#ifndef __SDIO_H__
#define __SDIO_H__

#define SDIO_CMD_GPIO   D
#define SDIO_CMD_PIN    2
#define SDIO_CK_GPIO    C
#define SDIO_CK_PIN     12
#define SDIO_D0_GPIO    C
#define SDIO_D0_PIN     8
#define SDIO_D1_GPIO    C
#define SDIO_D1_PIN     9
#define SDIO_D2_GPIO    C
#define SDIO_D2_PIN     10
#define SDIO_D3_GPIO    C
#define SDIO_D3_PIN     11
#define SDIO_D4_GPIO    B
#define SDIO_D4_PIN     8
#define SDIO_D5_GPIO    B
#define SDIO_D5_PIN     9
#define SDIO_D6_GPIO    C
#define SDIO_D6_PIN     6
#define SDIO_D7_GPIO    C
#define SDIO_D7_PIN     7

void sdio_init(void);
void sdio_write(uint8_t *data, uint32_t sz);

#endif
