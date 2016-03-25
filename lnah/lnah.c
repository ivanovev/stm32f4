
#include <main.h>
#include "lnah/lnah.h"

#define LNAH_ALM_U1         (1 << 1)
#define LNAH_ALM_U2         (1 << 2)
#define LNAH_ALM_U3         (1 << 3)
#define LNAH_ALM_U4         (1 << 4)
#define LNAH_ALM_I1         (1 << 5)
#define LNAH_ALM_I2         (1 << 6)
#define LNAH_ALM_I3         (1 << 0)

#define EEPROM_U1_ADDR      0x100
#define EEPROM_U2_ADDR      0x104
#define EEPROM_U3_ADDR      0x108
#define EEPROM_U4_ADDR      0x10C
#define EEPROM_I1_ADDR      0x110
#define EEPROM_I2_ADDR      0x114
#define EEPROM_I3_ADDR      0x118
#define EEPROM_KMI_ADDR     0x120
#define EEPROM_UMI0_ADDR    0x124

#ifdef ENABLE_VFD
#include "vfd/vfd_menu.h"

extern vfd_menu_state_t   *pstate;
extern vfd_menu_item_t    *pmain;
#endif

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
    adc_tim_config(600);
}

static volatile uint32_t g_acci = 0, g_accu = 0, g_umi = 0, g_umu = 0, g_outen = 1, g_alarms = 0;
static volatile uint32_t g_thru1 = 700, g_thru2 = 2300, g_thru3 = 2500, g_thru4 = 3600;
static volatile uint32_t g_thri1 = 60, g_thri2 = 200, g_thri3 = 12000, g_umi0 = 0;
static volatile float g_kmi = 1.000;

#ifdef ENABLE_PCL
COMMAND(outen) {
    if(argc > 1)
    {
        g_outen = str2int(argv[1]);
        if(g_outen)
            g_alarms = 0;
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

COMMAND(alarms) {
    return picolSetHex1Result(i, g_alarms);
}

COMMAND(alarm) {
    ARITY(argc >= 2, "alarm n 1|0");
    uint32_t n = str2int(argv[1]);
    if(n == 2)
        return picolSetIntResult(i, (g_alarms & (1 << n)) ? 1 : 0);
    uint32_t v = str2int(argv[2]);
    if(v)
        g_alarms |= (1 << n);
    else
        g_alarms &= ~(1 << n);
    return picolSetIntResult(i, v);
}

COMMAND(umi) {
    volatile uint32_t *ptr = 0;
    volatile uint16_t addr = 0;
    if(SUBCMD0("umi") && (argc == 1))
        ptr = &g_umi;
    if(SUBCMD0("umi0"))
    {
        ptr = &g_umi0;
        addr = EEPROM_UMI0_ADDR;
    }
    if(SUBCMD0("thri1"))
    {
        ptr = &g_thri1;
        addr = EEPROM_I1_ADDR;
    }
    if(SUBCMD0("thri2"))
    {
        ptr = &g_thri2;
        addr = EEPROM_I2_ADDR;
    }
    if(SUBCMD0("thri3"))
    {
        ptr = &g_thri3;
        addr = EEPROM_I2_ADDR;
    }
    if(!ptr)
        return PICOL_ERR;
    if(argc >= 2)
        *ptr = (uint32_t)(atof(argv[1])*10);
    double f = (double)*ptr;
    char buf[16];
    double2str(buf, sizeof(buf), f/10, "1");
#ifdef ENABLE_I2C
    if(addr && (argc >= 2))
        eeprom_write_data(addr, (uint8_t*)ptr, 4);
#endif
    return picolSetResult(i, buf);
}

COMMAND(kmi) {
    if(argc == 2)
        g_kmi = atof(argv[1]);
    char buf[16];
    double2str(buf, sizeof(buf), g_kmi, "6");
#ifdef ENABLE_I2C
    eeprom_write_data(EEPROM_KMI_ADDR, (uint8_t*)&g_kmi, 4);
#endif
    return picolSetResult(i, buf);
}

COMMAND(umu) {
    volatile uint32_t *ptr = 0;
    volatile uint16_t addr = 0;
    if(SUBCMD0("umu") && (argc == 1))
        ptr = &g_umu;
    if(SUBCMD0("thru1"))
    {
        ptr = &g_thru1;
        addr = EEPROM_U1_ADDR;
    }
    if(SUBCMD0("thru2"))
    {
        ptr = &g_thru2;
        addr = EEPROM_U2_ADDR;
    }
    if(SUBCMD0("thru3"))
    {
        ptr = &g_thru3;
        addr = EEPROM_U3_ADDR;
    }
    if(SUBCMD0("thru4"))
    {
        ptr = &g_thru4;
        addr = EEPROM_U4_ADDR;
    }
    if(!ptr)
        return PICOL_ERR;
    if(argc >= 2)
        *ptr = (uint32_t)(atof(argv[1])*100);
    double f = (double)*ptr;
    char buf[16];
    double2str(buf, sizeof(buf), f/100, "2");
#ifdef ENABLE_I2C
    if(addr && (argc >= 2))
        eeprom_write_data(addr, (uint8_t*)ptr, 4);
#endif
    return picolSetResult(i, buf);
}

COMMAND(status) {
    char buf[20];
    mysnprintf(buf, sizeof(buf), "%d %d 0x%2X", g_umi/10, g_umu/100, g_alarms);
    return picolSetResult(i, buf);
}

uint32_t lnah_counter = 0;
COMMAND(lnah) {
    return picolSetIntResult(i, lnah_counter);
}

#ifdef ENABLE_VFD
extern vfd_menu_state_t *pstate;
vfd_menu_item_t         *g_vfdouten = 0, *g_vfdstatus = 0;
#endif

void pcl_extra_init(picolInterp *i)
{
    picolRegisterCmd(i, "outen", picol_outen, 0);
    picolRegisterCmd(i, "umi", picol_umi, 0);
    picolRegisterCmd(i, "umi0", picol_umi, 0);
    picolRegisterCmd(i, "thri1", picol_umi, 0);
    picolRegisterCmd(i, "thri2", picol_umi, 0);
    picolRegisterCmd(i, "thri3", picol_umi, 0);
    picolRegisterCmd(i, "kmi", picol_kmi, 0);
    picolRegisterCmd(i, "umu", picol_umu, 0);
    picolRegisterCmd(i, "thru1", picol_umu, 0);
    picolRegisterCmd(i, "thru2", picol_umu, 0);
    picolRegisterCmd(i, "thru3", picol_umu, 0);
    picolRegisterCmd(i, "thru4", picol_umu, 0);
    picolRegisterCmd(i, "alarm", picol_alarm, 0);
    picolRegisterCmd(i, "alarms", picol_alarms, 0);
    picolRegisterCmd(i, "status", picol_status, 0);
    picolRegisterCmd(i, "acci", picol_acci, 0);
    picolRegisterCmd(i, "accu", picol_accu, 0);
    picolRegisterCmd(i, "lnah", picol_lnah, 0);
}
#endif

#if 0
extern struct picolInterp *pcl_interp;
static void lnah_pcl_init(void)
{
    picolRegisterCmd(pcl_interp, "outen", picol_outen, 0);
    picolRegisterCmd(pcl_interp, "umi", picol_umi, 0);
    picolRegisterCmd(pcl_interp, "umi0", picol_umi, 0);
    picolRegisterCmd(pcl_interp, "thri", picol_umi, 0);
    picolRegisterCmd(pcl_interp, "umu", picol_umu, 0);
    picolRegisterCmd(pcl_interp, "thru1", picol_umu, 0);
    picolRegisterCmd(pcl_interp, "thru2", picol_umu, 0);
    picolRegisterCmd(pcl_interp, "thru3", picol_umu, 0);
    picolRegisterCmd(pcl_interp, "thru4", picol_umu, 0);
    picolRegisterCmd(pcl_interp, "alarms", picol_alarms, 0);
    picolRegisterCmd(pcl_interp, "status", picol_status, 0);
    picolRegisterCmd(pcl_interp, "acci", picol_acci, 0);
    picolRegisterCmd(pcl_interp, "accu", picol_accu, 0);
    picolRegisterCmd(pcl_interp, "lnah", picol_lnah, 0);
}
#endif

#ifdef ENABLE_I2C
static void lnah_read_iu(uint16_t addr, uint32_t *ptr)
{
    uint32_t tmp = 0;
    if(eeprom_read_data(addr, (uint8_t*)&tmp, 4) == 4)
    {
        if(tmp != 0xFFFFFFFF)
        {
            *ptr = tmp;
        }
    }
}
#endif

void lnah_init()
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(OUTEN_GPIO, OUTEN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), GPIO_PIN_RESET);

#ifdef ENABLE_VFD
    g_vfdouten = vfd_menu_append_child(pmain, "Out Enable", "outen");
    vfd_menu_make_edit_int(g_vfdouten, 0, 1, 1, VFD_FLAG_IMMEDIATE);
    g_vfdouten->edit->flags |= VFD_FLAG_TIM_UPD;
#if 0
    item = vfd_menu_append_child(pmain, "I threshold, mA", "thri");
    vfd_menu_make_edit_int(item, 500, 1700, 100, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain, "U threshold1, V", "thru1");
    vfd_menu_make_edit_int(item, 7, 36, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain, "U threshold2, V", "thru2");
    vfd_menu_make_edit_int(item, 7, 36, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain, "U threshold3, V", "thru3");
    vfd_menu_make_edit_int(item, 7, 36, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain, "U threshold4, V", "thru4");
    vfd_menu_make_edit_int(item, 7, 36, 1, VFD_FLAG_IMMEDIATE);
#endif
    vfd_menu_item_t *item = vfd_menu_append_child(pmain->next, "Current, mA", "umi");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pmain->next, "Voltage, V", "umu");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pmain->next, "Alarms", "alarms");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    g_vfdstatus = vfd_menu_append_child(pmain->next, "Status(I U Alm)", "status");
    g_vfdstatus->edit->flags |= VFD_FLAG_TIM_UPD;
    vfd_menu_ok();
#endif
#ifdef ENABLE_I2C
    lnah_read_iu(EEPROM_U1_ADDR,    (uint32_t*)&g_thru1);
    lnah_read_iu(EEPROM_U2_ADDR,    (uint32_t*)&g_thru2);
    lnah_read_iu(EEPROM_U3_ADDR,    (uint32_t*)&g_thru3);
    lnah_read_iu(EEPROM_U4_ADDR,    (uint32_t*)&g_thru4);
    lnah_read_iu(EEPROM_I1_ADDR,    (uint32_t*)&g_thri1);
    lnah_read_iu(EEPROM_I2_ADDR,    (uint32_t*)&g_thri2);
    lnah_read_iu(EEPROM_I3_ADDR,    (uint32_t*)&g_thri3);
    lnah_read_iu(EEPROM_UMI0_ADDR,  (uint32_t*)&g_umi0);
    lnah_read_iu(EEPROM_KMI_ADDR,   (uint32_t*)&g_kmi);
#endif
    if(g_outen)
        HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), GPIO_PIN_SET);

    adc_start();
}

void lnah_data_upd(void)
{
    uint16_t *ptr = (uint16_t*)adc_get_data_ptr();
    if(!ptr)
        return;
    static uint32_t acci = 0, accu = 0;
    static uint32_t outen_counter = 0;
    uint32_t acci0 = 0, accu0 = 0;
    static uint32_t j = 0;
    uint16_t k = 0;
    for(k = 0; k < ADC_BUF_SZ/sizeof(uint16_t); k++)
    {
        if(k % 2)
            accu0 += ptr[k];
        else
            acci0 += ptr[k];
    }
    j += k/2;
    acci += acci0;
    accu += accu0;
    if(j >= 65536)
    {
        g_acci = acci;
        g_accu = accu;
        g_umi = acci >> 14;
        g_umu = accu >> 16;
        acci = 0;
        accu = 0;
        led_toggle();
        j = 0;
        //lnah_counter++;
        if(g_umi0 == 0)
            g_umi0 = g_umi;
        g_umi = (uint32_t)(g_kmi*g_umi);
        g_umi = (g_umi > g_umi0) ? (g_umi - g_umi0) : 0;
    }
    acci0 >>= 5;
    if(acci0 >= (g_thri3 + g_umi0))
    {
        HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), GPIO_PIN_RESET);
        g_outen = 0;
        g_alarms = LNAH_ALM_I3;
        return;
    }
    if((acci0 < (g_thri1 + g_umi0)) && (g_outen == 1))
        g_alarms |= LNAH_ALM_I1;
    else
        g_alarms &= ~LNAH_ALM_I1;
    if(acci0 > (g_thri2 + g_umi0))
        g_alarms |= LNAH_ALM_I2;
    else
        g_alarms &= ~LNAH_ALM_I2;
    if(g_outen == 0)
    {
        outen_counter = 0;
        return;
    }
    accu0 >>= 7;
    if(accu0 > g_thru4)
    {
        HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), GPIO_PIN_RESET);
        g_outen = 0;
        g_alarms = LNAH_ALM_U4;
        return;
    }
    if(accu0 > g_thru3)
        g_alarms |= LNAH_ALM_U3;
    else
        g_alarms &= ~LNAH_ALM_U3;
    if(outen_counter < 5000)
        outen_counter += 1;
    if(outen_counter < 5000)
        return;
    if(accu0 < g_thru1)
    {
        HAL_GPIO_WritePin(GPIO(OUTEN_GPIO), PIN(OUTEN_PIN), GPIO_PIN_RESET);
        g_outen = 0;
        g_alarms = LNAH_ALM_U1;
        return;
    }
    if(accu0 < g_thru2)
        g_alarms |= LNAH_ALM_U2;
    else
        g_alarms &= ~LNAH_ALM_U2;
}

#ifdef ENABLE_VFD
void vfd_menu_ok(void)
{
    if(pstate->sel == g_vfdstatus)
    {
        pstate->sel = g_vfdouten;
        pstate->scroll = g_vfdouten;
    }
    else
    {
        pstate->sel = g_vfdstatus;
        pstate->scroll = g_vfdstatus;
    }
    pstate->sel->edit->flags |= VFD_FLAG_EDIT;
    vfd_menu_draw();
}
#endif

