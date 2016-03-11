
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
    GPIO_INIT(ADC_GPIO_I, ADC_PIN_I, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
    GPIO_INIT(ADC_GPIO_U, ADC_PIN_U, GPIO_MODE_ANALOG, GPIO_NOPULL, 0, 0);
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

    scfg.Channel = ADC_CHANNEL_I;
    scfg.Rank = 1;
    scfg.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    scfg.Offset = 0;
    if(HAL_ADC_ConfigChannel(&hadc, &scfg) != HAL_OK)
    {
        Error_Handler();
    }

    if(hadc.Init.NbrOfConversion >= 2)
    {
        scfg.Channel = ADC_CHANNEL_U;
        scfg.Rank = 2;
        if(HAL_ADC_ConfigChannel(&hadc, &scfg) != HAL_OK)
        {
            Error_Handler();
        }
    }
    adc_tim_config(9999);
}

#ifdef ENABLE_PCL
static volatile uint32_t g_acci = 0, g_accu = 0, g_umi0 = 0, g_umu0 = 0, g_umi = 0, g_umu = 0, g_outen = 0;

COMMAND(outen) {
    if(argc > 1)
    {
        g_outen = str2int(argv[1]);
        HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), g_outen ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    return picolSetIntResult(i, g_outen);
}

COMMAND(acci) {
    return picolSetIntResult(i, g_acci);
}

COMMAND(accu) {
    return picolSetIntResult(i, g_accu);
}

COMMAND(umi0) {
    return picolSetIntResult(i, g_umi0);
}

COMMAND(umu0) {
    return picolSetIntResult(i, g_umu0);
}

COMMAND(umi) {
    char buf[16];
    mysnprintf(buf, sizeof(buf), "%d.%d", g_umi/10, g_umi % 10);
    return picolSetResult(i, buf);
}

COMMAND(umu) {
    char buf[16];
    mysnprintf(buf, sizeof(buf), "%d.%d%d", g_umu/100, (g_umu % 100)/10, g_umu % 10);
    return picolSetResult(i, buf);
}

uint32_t lnah_counter = 0;
COMMAND(lnah) {
    return picolSetIntResult(i, lnah_counter);
}


extern struct picolInterp *pcl_interp;
static void lnah_pcl_init(void)
{
    picolRegisterCmd(pcl_interp, "umi0", picol_umi0, 0);
    picolRegisterCmd(pcl_interp, "umu0", picol_umu0, 0);
    picolRegisterCmd(pcl_interp, "acci", picol_acci, 0);
    picolRegisterCmd(pcl_interp, "accu", picol_accu, 0);
    picolRegisterCmd(pcl_interp, "umi", picol_umi, 0);
    picolRegisterCmd(pcl_interp, "umu", picol_umu, 0);
    picolRegisterCmd(pcl_interp, "outen", picol_outen, 0);
    picolRegisterCmd(pcl_interp, "lnah", picol_lnah, 0);
}
#endif

void lnah_init()
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(OUTEN_GPIO, OUTEN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), GPIO_PIN_RESET);

#ifdef ENABLE_PCL
    lnah_pcl_init();
#endif

    vfd_menu_item_t *item = vfd_menu_append_child(pmain, "Out Enable", "outen");
    vfd_menu_make_edit_int(item, 0, 1, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain->next, "Current, mA", "umi");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pmain->next, "Voltage, V", "umu");
    item->edit->flags |= VFD_FLAG_TIM_UPD;

    adc_start();
}

#else
void lnah_init()
{
}
#endif

void lnah_data_upd(void)
{
    uint16_t *ptr = (uint16_t*)adc_get_data_ptr();
    if(!ptr)
        return;
    static uint32_t acci = 0, accu = 0;
    static uint16_t j = 0;
    uint16_t k = 0;
    for(k = 0; k < ADC_BUF_SZ/sizeof(uint16_t); k++)
    {
        if(k == 0)
            g_umi0 = ptr[k];
        if(k == 1)
            g_umu0 = ptr[k];
        if(k % 2)
            accu += ptr[k];
        else
            acci += ptr[k];
    }
    j += k/2;
    if(j >= 1024)
    {
        g_acci = acci;
        g_accu = accu;
        g_umi = acci >> 8;
        g_umu = accu >> 10;
        acci = 0;
        accu = 0;
        j = 0;
    }
    lnah_counter++;
}

