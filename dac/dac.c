
#include <main.h>
#include <dac/dac.h>

#pragma message "DAC_OUT: DAC" STR(DAC_OUTn)
#pragma message "DAC_TIM: TIM" STR(DAC_TIMn)
#pragma message "DAC_DMA: DMA" STR(DAC_DMAn)
#pragma message "DAC_DMA_STREAM: DMA_STREAM" STR(DAC_DMA_STREAMn)
#pragma message "DAC_DMA_CHANNEL: DMA_CHANNEL" STR(DAC_DMA_CHANNELn)

DAC_HandleTypeDef hdac;
static DAC_ChannelConfTypeDef scfg;

static void dac_tim_config(void);

void dac_init(void)
{
    hdac.Instance = DAC;
    dac_tim_config();
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

void dac_start(const uint8_t *data, uint16_t sz)
{
    if(HAL_DAC_Start_DMA(&hdac, DACx_CHANNEL, (uint32_t*)data, sz, DAC_ALIGN_8B_R) != HAL_OK)
    { 
        Error_Handler();
    }
}

void dac_stop(void)
{

}

static void dac_tim_config(void)
{
    static TIM_HandleTypeDef  htim;
    TIM_MasterConfigTypeDef smastercfg;

    htim.Instance = DAC_TIMx;
    //htim.Init.Period = 0x0FF;
    htim.Init.Period = 99;
    htim.Init.Prescaler = 0;
    htim.Init.ClockDivision = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&htim) != HAL_OK)
    { 
        Error_Handler();
    }

    smastercfg.MasterOutputTrigger = TIM_TRGO_UPDATE;
    smastercfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    HAL_TIMEx_MasterConfigSynchronization(&htim, &smastercfg);

    if(HAL_TIM_Base_Start(&htim) != HAL_OK)
    { 
        Error_Handler();
    }
}

