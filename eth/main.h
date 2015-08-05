
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef ENABLE_UART
#include "uart/uart_dbg.h"
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "util/util.h"
#include "util/system_msp.h"

#if 0
#ifdef ENABLE_PTP
#define PTP_TIMn                2
#define PTP_TIMx                JOIN(TIM, PTP_TIMn)
#define PTP_TIMx_CLK_ENABLE     JOIN3(__TIM, PTP_TIMn, _CLK_ENABLE)
#define PTP_TIMx_IRQn           JOIN3(TIM, PTP_TIMn, _IRQn)
#define PTP_TIMx_CH1_GPIO       A
#define PTP_TIMx_CH1_PIN        5
#define PTP_TIMx_AF             GPIO_AF1_TIM2
#include "eth/eth.h"
#endif
#endif

#endif

