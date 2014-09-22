
#include <main.h>

extern ETH_HandleTypeDef heth;

void ETH_IRQHandler(void)
{
    HAL_ETH_IRQHandler(&heth);
}

