
#include "myptpd.h"

#define ADJ_FREQ_INC            43
#define ADJ_FREQ_ADDEND         0x4C19EF00
#define PTP_UPD_SIGN_NEGATIVE ETH_PTPTSLUR_TSUPNS

extern ETH_HandleTypeDef heth;

void eth_ptp_start(ETH_HandleTypeDef *pheth, uint16_t update_method)
{
    // step 1
    __HAL_ETH_MAC_DISABLE_IT(pheth, ETH_MAC_IT_TST);
    //pheth->Instance->MACIMR |= ETH_MAC_IT_TST;
    // step2; enable ptp timestamps for transmit and receive frames
    pheth->Instance->PTPTSCR |= ETH_PTPTSCR_TSE | ETH_PTPTSSR_TSSIPV4FE | ETH_PTPTSSR_TSSARFE | ETH_PTPTSSR_TSPTPPSV2E;
    // step 3;
    // 168/50 = 3.36
    // 43/3.36 = 13
    pheth->Instance->PTPSSIR = ADJ_FREQ_INC;
    // step 4; in case of fine update
    // 168/50 = 3.36
    // 2**32/3.36 = 0x4C30C30C
    if(update_method == PTP_FINE)
    {
        pheth->Instance->PTPTSAR = ADJ_FREQ_ADDEND;
        pheth->Instance->PTPTSCR |= ETH_PTPTSCR_TSARU;
        while(pheth->Instance->PTPTSCR & ETH_PTPTSCR_TSARU);
    }
    // step 6; set coarse update
    if(update_method == PTP_FINE)
        pheth->Instance->PTPTSCR |= ETH_PTPTSCR_TSFCU;
    else
        pheth->Instance->PTPTSCR &= ~ETH_PTPTSCR_TSFCU;
    // step 7
    pheth->Instance->PTPTSHUR = 0;
    pheth->Instance->PTPTSLUR = 0;
    // step 8
    pheth->Instance->PTPTSCR |= ETH_PTPTSCR_TSSTI;
    // step 9
    pheth->Instance->DMABMR |= ETH_DMABMR_EDE;

//#ifdef PTP_TIMx
#if 0
    TIM_HandleTypeDef htim;
    htim.Instance = PTP_TIMx;
    htim.Init.Period = 999999;
    htim.Init.Prescaler = (uint32_t) ((SystemCoreClock / 20000) - 1);
    htim.Init.Prescaler = tim_get_prescaler(PTP_TIMx)/100;
    htim.Init.ClockDivision = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_OC_Init(&htim) != HAL_OK)
    {
        Error_Handler();
    }
    TIM_ClockConfigTypeDef clk_src;
    clk_src.ClockSource = TIM_CLOCKSOURCE_ITR1;
    clk_src.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
    clk_src.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
    clk_src.ClockFilter = 0;
    if(HAL_TIM_ConfigClockSource(&htim, &sCLKSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode     = TIM_OCMODE_PWM1;
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.Pulse = 1000;
    if(HAL_TIM_OC_ConfigChannel(&htim, &oc_init, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if(HAL_TIM_OC_Start(&htim, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
#endif

#if 1
    PTP_TIMx->OR = TIM_OR_ITR1_RMP_0;
    eth_ptp_ppsfreq(6);
    //uint32_t *PTPPPSCR = (uint32_t*)&pheth->Instance->PTPTSSR + 1;
    //*PTPPPSCR = 6;
#endif

    // pass control frames
    pheth->Instance->MACFFR = ETH_PASSCONTROLFRAMES_FORWARDALL | ETH_MULTICASTFRAMESFILTER_NONE;

    if(pheth->Init.RxMode == ETH_RXINTERRUPT_MODE)
        __HAL_ETH_DMA_ENABLE_IT(pheth, ETH_DMA_IT_NIS | ETH_DMA_IT_T | ETH_DMA_IT_R);
    else
        __HAL_ETH_DMA_ENABLE_IT(pheth, ETH_DMA_IT_NIS | ETH_DMA_IT_T);
    HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
}

int32_t eth_ptp_ppsfreq(int32_t ppsfreq)
{
    uint32_t *PTPPPSCR = (uint32_t*)&heth.Instance->PTPTSSR + 1;
    if(ppsfreq == -1)
        return *PTPPPSCR;
    //dbg_send_hex2("PTPPPSCR", (uint32_t)PTPPPSCR);
    *PTPPPSCR = ppsfreq;
    return *PTPPPSCR;
}

void eth_ptpclk_time(ptpts_t *ts)
{
    ts->s = heth.Instance->PTPTSHR;
    ts->ns = ptp_ss2ns(heth.Instance->PTPTSLR);
}

uint32_t eth_ptpclk_seconds(void)
{
    return heth.Instance->PTPTSHR % (60*60*24);
}

void eth_ptpts_coarse_update(ptpdt_t *dt)
{
    heth.Instance->PTPTSHUR = dt->s;
    uint32_t ss = ptp_ns2ss(dt->ns);
    if(dt->ns < 0)
        ss |= ETH_PTPTSLUR_TSUPNS;
    heth.Instance->PTPTSLUR = ss;
    heth.Instance->PTPTSCR |= ETH_PTPTSCR_TSSTU;
}

void eth_ptpfreq_adjust(int32_t adj)
{
    //dbg_send_int2("adj", adj);
    adj /= 1000;
    int32_t a = ADJ_FREQ_ADDEND / 10000000;
    a *= adj;
    //dbg_send_int2("a", a);
    heth.Instance->PTPTSAR = ADJ_FREQ_ADDEND + a;
    heth.Instance->PTPTSCR |= ETH_PTPTSCR_TSARU;
}

void eth_ptpts_get(ptpts_t *ptime, volatile ETH_DMADescTypeDef *pdmadesc)
{
    static ptpts_t time;
    if(pdmadesc)
    {
        time.s = pdmadesc->TimeStampHigh;
        time.ns = ptp_ss2ns(pdmadesc->TimeStampLow);
    }
    if(ptime)
    {
        ptime->s = time.s;
        ptime->ns = time.ns;
    }
}

void eth_ptpts_now(ptpts_t *pts)
{
    pts->s = heth.Instance->PTPTSHR;
    pts->ns = ptp_ss2ns(heth.Instance->PTPTSLR);
}

void eth_ptpts_rx(ptpts_t *pts, ETH_DMADescTypeDef *dmarxdesc)
{
    if((dmarxdesc->Status & ETH_DMARXDESC_MAMPCE) && (dmarxdesc->ExtendedStatus & ETH_DMAPTPRXDESC_PTPMT))
    {
        //dmarxdesc = heth.RxFrameInfos.LSRxDesc;
        //dbg_send_hex2("rx status", dmarxdesc->Status);
        //dbg_send_hex2("rx ext status", dmarxdesc->ExtendedStatus);
#if 0
        //uint16_t i;
        ptpts_t time;
        //eth_ptpts_now(&time);
        for(i = 0; i < ETH_RXBUFNB; i++)
        {
            eth_ptpts_get(&time, &DMARxDscrTab[i]);
            dbg_send_int2("rx.s", time.s);
            dbg_send_int2("rx.ns", time.ns);
        }
#endif
        eth_ptpts_get(0, dmarxdesc);
    }
}

