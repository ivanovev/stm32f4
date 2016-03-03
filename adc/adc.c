
#include <adc/adc.h>
#include "gpio/led.h"
#include "util/util.h"

#pragma message "ADC: ADC" STR(ADCn)
#pragma message "ADC_IN: ADC" STR(ADC_INn)

#ifdef ADC_DMAn
#pragma message "ADC_TIM: TIM" STR(ADC_TIMn)
#pragma message "ADC_DMA: DMA" STR(ADC_DMAn)
#pragma message "ADC_DMA_STREAM: DMA_STREAM" STR(ADC_DMA_STREAMn)
#pragma message "ADC_DMA_CHANNEL: DMA_CHANNEL" STR(ADC_DMA_CHANNELn)
#endif

#define ADC_BUF_NB  2
#define ADC_BUF_SZ  IO_BUF_SZ

__ALIGN_BEGIN uint8_t ADC_Buff[ADC_BUF_NB][ADC_BUF_SZ] __ALIGN_END;

struct adcdata_t
{
    uint8_t *buf0;
    uint8_t *buf1;
    volatile uint32_t sz;
    volatile uint32_t counter;
    volatile uint8_t convcplt;
} adcd;

ADC_HandleTypeDef hadc;
static ADC_ChannelConfTypeDef scfg;

#ifdef ADC_TIMn
static void adc_tim_config(void);
#endif

void adc_init(void)
{
#ifdef ADCn
    adc_hadc_init(&hadc);
    if(HAL_ADC_Init(&hadc) != HAL_OK)
    {
        Error_Handler();
    }
#endif
#ifdef ADCx_CHANNEL
    scfg.Channel = ADCx_CHANNEL;
    scfg.Rank = 1;
    scfg.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    scfg.Offset = 0;

    if(HAL_ADC_ConfigChannel(&hadc, &scfg) != HAL_OK)
    {
        Error_Handler();
    }
#endif
#ifdef ADC_TIMn
    adc_tim_config();
#endif
}

void adc_hadc_init(ADC_HandleTypeDef *phadc)
{
    phadc->Instance = ADCx;
    phadc->Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	phadc->Init.Resolution            = ADC_RESOLUTION_12B;
    phadc->Init.ScanConvMode = DISABLE;
	phadc->Init.ContinuousConvMode    = DISABLE;
    phadc->Init.DiscontinuousConvMode = DISABLE;
    phadc->Init.NbrOfConversion = 1;
	phadc->Init.NbrOfDiscConversion   = 0;
#ifdef ADC_DMAn
    adcd.buf0 = &(ADC_Buff[0][0]);
    adcd.buf1 = &(ADC_Buff[1][0]);
    mymemset(adcd.buf0, 0, ADC_BUF_SZ);
    mymemset(adcd.buf1, 0, ADC_BUF_SZ);
    adcd.counter = 0;
    adcd.convcplt = 0;

    //hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    //hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc.Init.ExternalTrigConv = ADCx_TRIGGER;
    hadc.Init.DataAlign = ADC_DATAALIGN_LEFT;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
#else
	hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.EOCSelection          = DISABLE;
#endif
}

void adc_start(void)
{
    if(HAL_ADC_Start_DMA(&hadc, (uint32_t*)adcd.buf0, ADC_BUF_NB*ADC_BUF_SZ/sizeof(uint16_t)) != HAL_OK)
    {
        Error_Handler();
    }
}

void adc_start_sz(uint32_t sz)
{
    adcd.counter = 0;
    adcd.sz = sz/ADC_BUF_SZ;
    adc_start();
}

void adc_stop(void)
{
    HAL_ADC_Stop_DMA(&hadc);
}

uint16_t adc_get_data(uint8_t *out, uint16_t sz)
{
    if(adcd.convcplt)
    {
        uint16_t *ptr1 = (uint16_t*)((adcd.counter % 2) ? adcd.buf0 : adcd.buf1);
        uint16_t *ptr2 = (uint16_t*)out;
        sz = MIN(sz, ADC_BUF_SZ);
        uint16_t i;
        // copy to out and convert unsigned to signed
        for(i = 0; i < sz; i++)
            *ptr2++ = *ptr1++ + 0x8000;
        //mymemcpy(out, ptr, sz);
        adcd.convcplt = 0;
        return sz;
    }
    return 0;
}

#ifdef ADC_TIMn
static void adc_tim_config(void)
{
    static TIM_HandleTypeDef  htim_adc;
    TIM_MasterConfigTypeDef smastercfg;

    htim_adc.Instance = ADC_TIMx;
    htim_adc.Init.Period = 999;
    htim_adc.Init.Prescaler = 0;
    htim_adc.Init.ClockDivision = 0;
    htim_adc.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&htim_adc) != HAL_OK)
    {
        Error_Handler();
    }

    smastercfg.MasterOutputTrigger = TIM_TRGO_UPDATE;
    smastercfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    HAL_TIMEx_MasterConfigSynchronization(&htim_adc, &smastercfg);

    if(HAL_TIM_Base_Start(&htim_adc) != HAL_OK)
    {
        Error_Handler();
    }
}
#endif

#if 1
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* phadc)
{
    adcd.counter++;
    adcd.convcplt = 1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* phadc)
{
    adcd.counter++;
    adcd.convcplt = 1;
    if(adcd.sz)
    {
        if(adcd.counter >= adcd.sz)
        {
            adc_stop();
            adcd.counter = 0;
            adcd.sz = 0;
        }
    }
}
#endif

uint16_t adc_read(void)
{
    uint16_t data = 0;
    if(HAL_ADC_Start(&hadc) != HAL_OK)
    {
        return -1;
    }

    HAL_ADC_PollForConversion(&hadc, 10);

    if((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
    {
        data = HAL_ADC_GetValue(&hadc);
    }
    return data;
}

