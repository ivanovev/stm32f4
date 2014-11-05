
#ifndef __MYINIT_H__
#define __MYINIT_H__

#include "stdint.h"
#include "stddef.h"

#include <main.h>
#include "gpio/gpio.h"
#include "flash/flash.h"
#include "pcl/pcl.h"
#include "tim/tim.h"
#include "i2c/i2c.h"
#include "uart/uart.h"
#include "vfd/vfd.h"
#ifdef MY_ETH
#include "eth/eth.h"
#endif
#ifdef MY_USB
extern void usb_init();
#endif

#define RESET_REBOOT 1
#define RESET_FWUPG 2
#define RESET_PCLUPD 3

void myinit(void);
void mydeinit(void);

#endif

