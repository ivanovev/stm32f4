
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef ENABLE_UART
#include "uart/uart_dbg.h"
#endif

#define ALT_UARTn 2
#define ALT_UART_TX_GPIO D
#define ALT_UART_RX_GPIO D
#define ALT_UART_TX_PIN 5
#define ALT_UART_RX_PIN 6
#define ALT_UART_BAUDRATE 38400

#include "util/system_msp.h"
#include "util/util.h"

#endif

