
#ifndef __UART_DBG_H__
#define __UART_DBG_H__

#if 1
#ifdef BOARD_E407
#define UARTn 6
#define UART_TX_GPIO C
#define UART_RX_GPIO C
#define UART_TX_PIN 6
#define UART_RX_PIN 7
#endif
#ifdef BOARD_HYSTM32
#define UARTn 4
#define UART_TX_GPIO A
#define UART_RX_GPIO A
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#endif
#ifdef BOARD_MY1
#define UARTn 3
#define UART_TX_GPIO D
#define UART_RX_GPIO D
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#endif
#else
#define UARTn 5
#define UART_TX_GPIO C
#define UART_TX_PIN 12
#define UART_RX_GPIO D
#define UART_RX_PIN 2
#endif

#define UART_BAUDRATE 115200
//#define UART_RX_DISABLE

#endif

