
#include <main.h>
#include "eth/eth.h"
#include "eth/mdio.h"

extern void eth_reset(void);
extern ETH_HandleTypeDef heth;
#ifdef ENABLE_PTP
#include "eth/myip/ptp/ptpd.h"
#endif

#pragma message "ETH_MDC_GPIO: GPIO" STR(ETH_MDC_GPIO) " PIN" STR(ETH_MDC_PIN)
#pragma message "ETH_MDIO_GPIO: GPIO" STR(ETH_MDIO_GPIO) " PIN" STR(ETH_MDIO_PIN)
#pragma message "ETH_RESET_GPIO: GPIO" STR(ETH_RESET_GPIO) " PIN" STR(ETH_RESET_PIN)
#pragma message "ETH_REFCLK_GPIO: GPIO" STR(ETH_REFCLK_GPIO) " PIN" STR(ETH_REFCLK_PIN)
#pragma message "ETH_CRSDV_GPIO: GPIO" STR(ETH_CRSDV_GPIO) " PIN" STR(ETH_CRSDV_PIN)
#pragma message "ETH_RXD0_GPIO: GPIO" STR(ETH_RXD0_GPIO) " PIN" STR(ETH_RXD0_PIN)
#pragma message "ETH_RXD1_GPIO: GPIO" STR(ETH_RXD1_GPIO) " PIN" STR(ETH_RXD1_PIN)
#pragma message "ETH_TXEN_GPIO: GPIO" STR(ETH_TXEN_GPIO) " PIN" STR(ETH_TXEN_PIN)
#pragma message "ETH_TXD0_GPIO: GPIO" STR(ETH_TXD0_GPIO) " PIN" STR(ETH_TXD0_PIN)
#pragma message "ETH_TXD1_GPIO: GPIO" STR(ETH_TXD1_GPIO) " PIN" STR(ETH_TXD1_PIN)

void HAL_ETH_MspInit(ETH_HandleTypeDef *pheth)
{
#if 1
    GPIO_InitTypeDef gpio_init;

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __GPIOH_CLK_ENABLE();

    GPIO_INIT(ETH_MDC_GPIO, ETH_MDC_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_MDIO_GPIO, ETH_MDIO_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    GPIO_INIT(ETH_RESET_GPIO, ETH_RESET_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FAST, 0);
    HAL_GPIO_WritePin(GPIO(ETH_RESET_GPIO), PIN(ETH_RESET_PIN), GPIO_PIN_SET);

    //GPIO_INIT(ETH_NINT_GPIO, ETH_NINT_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);

    GPIO_INIT(ETH_REFCLK_GPIO, ETH_REFCLK_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    GPIO_INIT(ETH_CRSDV_GPIO, ETH_CRSDV_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_RXD0_GPIO, ETH_RXD0_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_RXD1_GPIO, ETH_RXD1_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    GPIO_INIT(ETH_TXEN_GPIO, ETH_TXEN_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_TXD0_GPIO, ETH_TXD0_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    GPIO_INIT(ETH_TXD1_GPIO, ETH_TXD1_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);

    __ETH_CLK_ENABLE();

#ifdef ENABLE_PTP
    GPIO_INIT(ETH_PPS_OUT_GPIO, ETH_PPS_OUT_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF11_ETH);
    __ETHMACPTP_CLK_ENABLE();
#endif

    dbg_send_str3("HAL_ETH_MspInit", 1);
    eth_reset();
#endif
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef *pheth)
{
    dbg_send_str3("HAL_ETH_MspDeInit", 1);
    __ETH_CLK_DISABLE();
}

#if 0
#ifdef ENABLE_PTP

#define ADJ_FREQ_INC            43
#define ADJ_FREQ_ADDEND         0x4C19EF00
#define PTP_UPD_SIGN_NEGATIVE ETH_PTPTSLUR_TSUPNS

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
    dbg_send_hex2("PTPPPSCR", PTPPPSCR);
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

#endif
#endif
