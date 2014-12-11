
#include <main.h>

void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
    __RNG_CLK_ENABLE();
}

void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng)
{
    __RNG_FORCE_RESET();
    __RNG_RELEASE_RESET();
}

