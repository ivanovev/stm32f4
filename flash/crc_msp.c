
#include <main.h>

void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
    /* CRC Peripheral clock enable */
    __CRC_CLK_ENABLE();
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
    /* Enable CRC reset state */
    __CRC_FORCE_RESET();

    /* Release CRC from reset state */
    __CRC_RELEASE_RESET();
}

