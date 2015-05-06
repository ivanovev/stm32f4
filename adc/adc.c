
#include <adc/adc.h>

#pragma message "ADC: ADC" STR(ADCn)
#pragma message "ADC_IN: ADC" STR(ADC_INn)
#pragma message "ADC_TIM: TIM" STR(ADC_TIMn)
#pragma message "ADC_DMA: DMA" STR(ADC_DMAn)
#pragma message "ADC_DMA_STREAM: DMA_STREAM" STR(ADC_DMA_STREAMn)
#pragma message "ADC_DMA_CHANNEL: DMA_CHANNEL" STR(ADC_DMA_CHANNELn)

#define ADC_BUF_NB  2
#define ADC_BUF_SZ  512

__ALIGN_BEGIN uint8_t ADC_Buff[ADC_BUF_NB][ADC_BUF_SZ] __ALIGN_END;

uint8_t *ADC_Buf0 = &ADC_Buff[0][0];
uint8_t *ADC_Buf1 = &ADC_Buff[1][0];

ADC_HandleTypeDef hadc;
static ADC_ChannelConfTypeDef scfg;

static void adc_tim_config(void);

void adc_init(void)
{
    hadc.Instance = ADCx;
    adc_tim_config();

    hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.ScanConvMode = DISABLE;
    hadc.Init.ContinuousConvMode = ENABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.NbrOfDiscConversion = 0;
    //hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc.Init.ExternalTrigConv = ADCx_TRIGGER;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.NbrOfConversion = 1;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.EOCSelection = DISABLE;

    if(HAL_ADC_Init(&hadc) != HAL_OK)
    {
        Error_Handler();
    }

    scfg.Channel = ADCx_CHANNEL;
    scfg.Rank = 1;
    scfg.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    scfg.Offset = 0;

    if(HAL_ADC_ConfigChannel(&hadc, &scfg) != HAL_OK)
    {
        Error_Handler();
    }


}

void adc_start(uint8_t *buf1, uint8_t *buf2, uint16_t sz)
{
    if(HAL_ADC_Start_DMA(&hadc, (uint32_t*)ADC_Buf0, ADC_BUF_SZ/sizeof(uint32_t)) != HAL_OK)
    {
        Error_Handler();
    }
}

void adc_stop(void)
{

}

uint16_t adc_get_data(uint8_t *buf1, uint16_t sz)
{
    return 0;
}

static void adc_tim_config(void)
{
    static TIM_HandleTypeDef  htim;
    TIM_MasterConfigTypeDef smastercfg;

    htim.Instance = ADC_TIMx;
    htim.Init.Period = 0x0FF;
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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *phadc)
{
}

