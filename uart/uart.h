
#ifndef __UART_H__
#define __UART_H__

#include "main.h"

#ifdef BOARD_E407
#define UARTn 5
#define UART_TX_GPIO C
#define UART_RX_GPIO D
#define UART_TX_PIN 12
#define UART_RX_PIN 2
#endif
#ifdef BOARD_HYSTM32
#define UARTn 4
#define UART_TX_GPIO A
#define UART_RX_GPIO A
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#endif

#define UART_BAUDRATE 115200

#if UARTn == 4 || UARTn == 5
#define UARTx JOIN(UART, UARTn)
#define UARTx_IRQn JOIN3(UART, UARTn, _IRQn)
#define UARTx_CLK_ENABLE JOIN3(__UART, UARTn, _CLK_ENABLE)
#define UARTx_IRQHandler JOIN3(UART, UARTn, _IRQHandler)
#else
#define UARTx JOIN(USART, UARTn)
#define UARTx_IRQn JOIN3(USART, UARTn, _IRQn)
#define UARTx_CLK_ENABLE() JOIN3(__USART, UARTn, _CLK_ENABLE)
#define UARTx_IRQHandler JOIN3(USART, UARTn, _IRQHandler)
#endif

#if UARTn == 1 || UARTn == 2 || UARTn == 3
#define GPIO_AF_UART GPIO_AF7_USART1
#else
#define GPIO_AF_UART GPIO_AF8_UART4
#endif

void uart_init(void);
uint32_t uart_recv_str(char *buf);
void uart_send_str(const char *str, uint32_t len);
void uart_send_str2(const char *str);
void uart_send_str3(const char *str, uint8_t newline);
void uart_send_int(int n);
void uart_send_int2(char *str, int n);
void uart_send_hex(unsigned int h, uint8_t bytes);
void uart_send_hex2(char *str, unsigned int h);
void uart_send_hex3(char *str, int n, unsigned int h);
void uart_send_hex4(const char *str, uint8_t *bb, uint32_t n);

#endif

