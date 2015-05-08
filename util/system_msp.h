
#ifndef __MYINIT_H__
#define __MYINIT_H__

#include "stdint.h"
#include "stddef.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "util/macro.h"

#include <main.h>
#ifdef ENABLE_GPIO
#include "gpio/gpio.h"
#endif

#ifdef ENABLE_FLASH
#include "flash/flash.h"
#endif

#ifdef ENABLE_TIM
#include "tim/tim.h"
#endif

#ifdef ENABLE_ADC
#include "adc/adc.h"
#endif

#ifdef ENABLE_DAC
#include "dac/dac.h"
#endif

#ifdef ENABLE_DMA
#include "dma/dma.h"
#endif

#ifdef ENABLE_I2C
#include "i2c/i2c.h"
#endif

#ifdef ENABLE_UART
#include "uart/uart.h"
#endif

#ifdef ENABLE_VFD
#include "vfd/vfd.h"
#endif

#ifdef ENABLE_RNG
#include "rng/rng.h"
#endif

#ifdef ENABLE_PCL
#include "pcl/pcl.h"
#endif

#ifdef ENABLE_ETH
#include "eth/eth.h"
#endif

#ifdef ENABLE_DSP
#include "dsp/dsp.h"
#endif

#ifdef ENABLE_USB
#include "usb/usb.h"
#endif

#define RESET_REBOOT 1
#define RESET_FWUPG 2
#define RESET_PCLUPD 3

extern void Error_Handler(void);

void        myinit(void);
void        mydeinit(void);
uint32_t    uptime(void);

#endif

