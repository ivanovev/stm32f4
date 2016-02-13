
#ifndef __MYINIT_H__
#define __MYINIT_H__

#include "stdint.h"
#include "stddef.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "util/macro.h"

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

#ifdef ENABLE_SPI
#include "spi/spi.h"
#endif

#ifdef ENABLE_UART
#include "uart/uart.h"
#endif

#ifdef ENABLE_SDIO
#include "sdio/sdio.h"
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

#ifdef ENABLE_BR
#include "br/br.h"
#endif

#define EVT_REBOOT  (1 << 0)
#define EVT_FWUPG   (1 << 1)
#define EVT_PCLUPD  (1 << 2)
#define EVT_BTN     (1 << 3)

extern void Error_Handler(void);

void        myinit(void);
void        mydeinit(void);
uint32_t    uptime(void);

#endif

