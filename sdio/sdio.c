
#include <main.h>

static SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

void sdio_init(void)
{
    dbg_send_str2("sdio_init");
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = 0;
    if(HAL_SD_Init(&hsd, &SDCardInfo) != SD_OK)
    {
        dbg_send_str2("HAL_SD_Init error");
        return;
    }

    HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);
}

void sdio_write(uint8_t *data, uint32_t sz)
{
    HAL_SD_WriteBlocks_DMA(&hsd, (uint32_t*)data, 0, 512, sz/512);
}

