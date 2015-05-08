
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

static const uint8_t sin84[84] = {128,136,144,152,161,168,176,184,191,197,204,210,215,220,224,228,232,235,237,238,239,240,239,238,237,235,232,228,224,220,215,210,204,197,191,183,176,168,161,152,144,136,128,120,112,104,95,88,80,72,65,59,52,46,41,36,32,28,24,21,19,18,17,16,17,18,19,21,24,28,32,36,41,46,52,59,65,73,80,88,95,104,112,120};

void dac_start_sin(void)
{
    dac_start(sin84, sizeof(sin84));
}

void dac_stop(void)
{
    HAL_DAC_Stop_DMA(&hdac, DACx_CHANNEL);
}

static void dac_tim_config(void)
{
    static TIM_HandleTypeDef  htim;
    TIM_MasterConfigTypeDef smastercfg;

    htim.Instance = DAC_TIMx;
    //htim.Init.Period = 0x0FF;
    htim.Init.Period = 999;
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

