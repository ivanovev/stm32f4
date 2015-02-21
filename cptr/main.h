
#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "util/util.h"
#include "util/queue.h"
#include "util/system_msp.h"

#ifdef ENABLE_USB
#include "usbd_conf.h"
#include "usbd_core.h"
#endif

#define ETH_REFCLK_GPIO A
#define ETH_REFCLK_PIN 1
#define ETH_MDC_GPIO C
#define ETH_MDC_PIN 1
#define ETH_MDIO_GPIO A
#define ETH_MDIO_PIN 2

#define ETH_CRSDV_GPIO A
#define ETH_CRSDV_PIN 7
#define ETH_RXD0_GPIO C
#define ETH_RXD0_PIN 4
#define ETH_RXD1_GPIO C
#define ETH_RXD1_PIN 5

#ifdef ENABLE_ETH
#define ETH_RESET_GPIO C
#define ETH_RESET_PIN 3

#define ETH_TXEN_GPIO B
#define ETH_TXEN_PIN 11
#define ETH_TXD0_GPIO B
#define ETH_TXD0_PIN 12
#define ETH_TXD1_GPIO B
#define ETH_TXD1_PIN 13
#endif

#ifdef ENABLE_UART
#define UART_RX_DISABLE

#if 1
#define UARTn   4
#define UART_BAUDRATE 57600
#define UART_TX_GPIO C
#define UART_RX_GPIO C
#define UART_TX_PIN 10
#define UART_RX_PIN 11
#else
#define UARTn 4
#define UART_BAUDRATE 115200
#define UART_TX_GPIO A
#define UART_RX_GPIO A
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#endif
#endif

#endif

