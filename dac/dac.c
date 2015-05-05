
#include <main.h>
#include <dac/dac.h>

DAC_HandleTypeDef hdac;
static DAC_ChannelConfTypeDef scfg;
const uint8_t saw[16] = {0x0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0};

void dac_init(void)
{
    if(HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }
    scfg.DAC_Trigger = DACx_TRIGGER;
    scfg.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if(HAL_DAC_ConfigChannel(&hdac, &scfg, DACx_CHANNEL) != HAL_OK)
    { 
        Error_Handler();
    }
}

uint16_t dac_start(uint16_t *data, uint16_t sz)
{
    if(HAL_DAC_Start_DMA(&hdac, DACx_CHANNEL, (uint32_t*)saw, sizeof(saw), DAC_ALIGN_8B_R) != HAL_OK)
    { 
        Error_Handler();
    }
    return 0;
}

void dac_stop(void)
{

}

