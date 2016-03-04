
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef ENABLE_UART
#include "uart/uart_dbg.h"
#endif

#define ADCn                1
#define ADC_DMAn            2
#define ADC_DMA_STREAMn     0
#define ADC_DMA_CHANNELn    0
#define ADC_TIMn            2

#define ADC_BUF_SZ          IO_BUF_SZ

#define DAC_OUTn            1

#include "util/system_msp.h"
#include "util/util.h"

#endif

