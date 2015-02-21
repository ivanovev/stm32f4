
#ifndef __UART_H__
#define __UART_H__

#include <main.h>

#if UARTn == 4 || UARTn == 5
#define UARTx_type UART
#define UARTx_CLK_ENABLE JOIN3(__UART, UARTn, _CLK_ENABLE)
#else
#define UARTx_type USART
#define UARTx_CLK_ENABLE JOIN3(__USART, UARTn, _CLK_ENABLE)
#endif

#define UARTx JOIN(UARTx_type, UARTn)
#define UARTx_IRQn JOIN3(UARTx_type, UARTn, _IRQn)
#define UARTx_IRQHandler JOIN3(UARTx_type, UARTn, _IRQHandler)

#if UARTn == 1 || UARTn == 2 || UARTn == 3
#define GPIO_AF_UARTx JOIN(GPIO_AF7_USART, UARTn)
#endif
#if UARTn == 4 || UARTn == 5
#define GPIO_AF_UARTx JOIN(GPIO_AF8_UART, UARTn)
#endif
#if UARTn == 6
#define GPIO_AF_UARTx JOIN(GPIO_AF8_USART, UARTn)
#endif

void            uart_init(void);
USART_TypeDef*  uart_get_instance(uint8_t n);
void            uart_get_handle(UART_HandleTypeDef *phuart, uint8_t n);

uint16_t        uart_recv_str(char *buf);
void            uart_send_str(const char *str, uint16_t len);

volatile uint32_t* uart_get_reg_ptr(USART_TypeDef *uartx, const char *reg);
uint32_t        uart_get_reg(USART_TypeDef *uartx, const char *reg);
uint32_t        uart_set_reg(USART_TypeDef *uartx, const char *reg, uint32_t val);

#if 1
void uart_send_str2(const char *str);
void uart_send_str3(const char *str, uint8_t newline);
void uart_send_int(int n);
void uart_send_int2(char *str, int n);
void uart_send_hex(unsigned int h, uint8_t bytes);
void uart_send_hex2(char *str, unsigned int h);
void uart_send_hex3(char *str, int n, unsigned int h);
void uart_send_hex4(const char *str, uint8_t *bb, uint32_t n);
#endif

#endif

