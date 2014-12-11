
#include "rng/rng.h"

RNG_HandleTypeDef hrng;
extern void Error_Handler(void);

void rng_init(void)
{
    hrng.Instance = RNG;
    if(HAL_RNG_Init(&hrng) != HAL_OK)
    {
        Error_Handler();
    }
}

void rng_deinit()
{
    HAL_RNG_DeInit(&hrng);
}

uint32_t rng_uint32(void)
{
    return HAL_RNG_GetRandomNumber(&hrng);
}

uint16_t rng_uint16(void)
{
    return (uint16_t)HAL_RNG_GetRandomNumber(&hrng);
}

void rng_macaddr(uint8_t *macaddr)
{
    uint32_t rv = rng_uint32();
    macaddr[0] = rv & 0xFF;
    macaddr[1] = (rv >> 8) & 0xFF;
    macaddr[2] = (rv >> 16) & 0xFF;
    macaddr[3] = (rv >> 24) & 0xFF;
    rv = rng_uint32();
    macaddr[4] = rv & 0xFF;
    macaddr[5] = (rv >> 8) & 0xFF;
}

