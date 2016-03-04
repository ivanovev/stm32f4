
#include <main.h>
#include "lnah/lnah.h"

#ifdef ENABLE_VFD
#include "vfd/vfd_menu.h"

extern vfd_menu_state_t   *pstate;
extern vfd_menu_item_t    *pmain;

extern ADC_HandleTypeDef hadc;
extern ADC_ChannelConfTypeDef scfg;

void adc_init(void)
{
    dbg_send_str3("lnah_adc_init", 1);
    adc_data_init();
    GPIO_InitTypeDef          gpio_init;
    GPIO_INIT(ADCx_GPIO_I, ADCx_PIN_I, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
    GPIO_INIT(ADCx_GPIO_U, ADCx_PIN_U, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
    hadc.Instance = ADCx;
    hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    hadc.Init.Resolution            = ADC_RESOLUTION_12B;
    hadc.Init.ScanConvMode = ENABLE;
    hadc.Init.ContinuousConvMode    = DISABLE;
    hadc.Init.NbrOfConversion = 2;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.NbrOfDiscConversion   = 0;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc.Init.ExternalTrigConv = ADCx_TRIGGER;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.DMAContinuousRequests = ENABLE;
    hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    if(HAL_ADC_Init(&hadc) != HAL_OK)
    {
        Error_Handler();
    }

    scfg.Channel = ADCx_CHANNEL_I;
    scfg.Rank = 1;
    scfg.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    scfg.Offset = 0;
    if(HAL_ADC_ConfigChannel(&hadc, &scfg) != HAL_OK)
    {
        Error_Handler();
    }

    if(hadc.Init.NbrOfConversion >= 2)
    {
        scfg.Channel = ADCx_CHANNEL_U;
        scfg.Rank = 2;
        if(HAL_ADC_ConfigChannel(&hadc, &scfg) != HAL_OK)
        {
            Error_Handler();
        }
    }
    adc_tim_config(9999);
}

void lnah_init()
{
    vfd_menu_item_t *item = vfd_menu_append_child(pmain, "Out Enable", "enable");
    vfd_menu_make_edit_int(item, 0, 1, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain->next, "Voltage, V", "udc");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pmain->next, "Current, A", "idc");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
}

#else
void lnah_init()
{
}
#endif

