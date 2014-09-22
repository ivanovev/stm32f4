
#ifndef __MYINIT_H__
#define __MYINIT_H__

#include "stdint.h"
#include "stddef.h"

#include <main.h>
#include "gpio/gpio.h"
#include "tim/tim.h"
#include "uart/uart.h"
#include "pcl/pcl.h"
#ifdef MY_ETH
#include "eth/eth.h"
#endif
#ifdef MY_USB
extern void usb_init();
#endif

void myinit(void);

#endif

