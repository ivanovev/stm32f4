
#include <main.h>
#include "cptr.h"
#ifdef ENABLE_PCL
#include "pcl/pcl.h"
#include "pcl_wifi.h"
#endif

#pragma message "WIFI_RESET: GPIO" STR(WIFI_RESET_GPIO) " PIN" STR(WIFI_RESET_PIN)
#pragma message "WIFI_NRELOAD: GPIO" STR(WIFI_NRELOAD_GPIO) " PIN" STR(WIFI_NRELOAD_PIN)

void cptr_init(void)
{
#if 1
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(WIFI_RESET_GPIO, WIFI_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_LOW, 0);
    HAL_GPIO_WritePin(GPIO(WIFI_NRELOAD_GPIO), PIN(WIFI_NRELOAD_PIN), GPIO_PIN_SET);
    GPIO_INIT(WIFI_NRELOAD_GPIO, WIFI_NRELOAD_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_LOW, 0);
    cptr_wifi_reset();
#ifdef ENABLE_PCL
    picolInterp *interp = pcl_get_interp();
    pcl_wifi_init(interp);
#endif
#endif
}

void cptr_wifi_reset(void)
{
    HAL_GPIO_WritePin(GPIO(WIFI_RESET_GPIO), PIN(WIFI_RESET_PIN), GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIO(WIFI_RESET_GPIO), PIN(WIFI_RESET_PIN), GPIO_PIN_SET);
}

void cptr_wifi_reload(void)
{
    HAL_GPIO_WritePin(GPIO(WIFI_NRELOAD_GPIO), PIN(WIFI_NRELOAD_PIN), GPIO_PIN_RESET);
    HAL_Delay(3500);
    HAL_GPIO_WritePin(GPIO(WIFI_NRELOAD_GPIO), PIN(WIFI_NRELOAD_PIN), GPIO_PIN_SET);
}

