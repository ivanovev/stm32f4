
#include "uart/uart.h"
#include "util/queue.h"
#include "gpio/led.h"

#ifdef UARTn
#pragma message "UART: UART" STR(UARTn)
#pragma message "UART_BAUDRATE: " STR(UART_BAUDRATE)

UART_HandleTypeDef huart;
Queue quart;
uint8_t uart_rx_buf[IO_BUF_SZ];

uint16_t uart_recv_str(char *buf)
{
    return dequeue(&quart, (uint8_t*)buf, 1);
}

#ifndef UART_RX_DISABLE
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *phuart)
{
    led_toggle();
    uint8_t ch = uart_rx_buf[0];
    if(ch != '\r')
        enqueue(&quart, ch);
    HAL_UART_Transmit(phuart, uart_rx_buf, 1, 0);
    HAL_UART_Receive_IT(phuart, uart_rx_buf, 1);
}
#endif

extern void Error_Handler(void);

void uart_init(void)
{
    //led_on();
    quart.head = 0;
    quart.tail = 0;
    uart_get_handle(&huart, UARTn, HAL_UART_STATE_RESET);

    if(HAL_UART_Init(&huart) != HAL_OK)
    {
        Error_Handler();
    }
#ifndef UART_RX_DISABLE
    if(HAL_UART_Receive_IT(&huart, uart_rx_buf, 1) != HAL_OK)
    {
        Error_Handler();
    }
#endif
}

void uart_send_str(const char *str, uint16_t len)
{
    HAL_UART_Transmit(&huart, (uint8_t*)str, len, 100);
}
#else
#pragma message "!!!no UARTn defined!!!"
void uart_init(void)
{
}
void uart_send_str(const char *str, uint16_t len)
{
}
#endif

void uart_send_str2(const char *str)
{
    uart_send_str(str, mystrnlen(str, IO_BUF_SZ));
}

void uart_send_str3(const char *str, uint8_t newline)
{
    if(newline)
        uart_send_str2("\n\r");
    uart_send_str2(str);
}

void uart_send_int(int n)
{
    char buf[10];
    itoa(n, buf);
    uart_send_str2(buf);
}

void uart_send_int2(char *str, int n)
{
    uart_send_str3(str, 1);
    uart_send_str2(": ");
    uart_send_int(n);
}

void uart_send_hex(unsigned int h, uint8_t bytes)
{
    char buf[15];
    itoh(h, buf, bytes);
    uart_send_str2(buf);
}

void uart_send_hex2(char *str, unsigned int h)
{
    uart_send_str3(str, 1);
    uart_send_str2(": ");
    uart_send_hex(h, 4);
}

void uart_send_hex3(char *str, int n, unsigned int h)
{
    uart_send_str3(str, 1);
    uart_send_int(n);
    uart_send_str(": ", 0);
    uart_send_hex(h, 4);
}

void uart_send_hex4(const char *str, uint8_t *bb, uint32_t n)
{
    uint32_t i;
    uart_send_str3(str, 1);
    uart_send_str2("(");
    uart_send_int(n);
    uart_send_str2("): ");
    for(i = 0; i < n; i++)
    {
        uart_send_hex(bb[i], 1);
        uart_send_str2(" ");
    }
}

USART_TypeDef* uart_get_instance(uint8_t n)
{
    if(n == 1)
        return USART1;
    if(n == 2)
        return USART2;
    if(n == 3)
        return USART3;
    if(n == 4)
        return UART4;
    if(n == 5)
        return UART5;
    if(n == 6)
        return USART6;
    return 0;
}

void uart_get_handle(UART_HandleTypeDef *phuart, uint8_t n, uint8_t state)
{
    phuart->Instance        = uart_get_instance(n);
#ifdef UART_BAUDRATE
    phuart->Init.BaudRate   = UART_BAUDRATE;
#endif
    phuart->Init.WordLength = UART_WORDLENGTH_8B;
    phuart->Init.StopBits   = UART_STOPBITS_1;
    phuart->Init.Parity     = UART_PARITY_NONE;
    phuart->Init.Mode       = UART_MODE_TX_RX;
    phuart->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    phuart->ErrorCode       = HAL_UART_ERROR_NONE;
    phuart->Lock            = 0;
    phuart->gState          = state;
    phuart->RxState         = HAL_UART_STATE_READY;
}

volatile uint32_t* uart_get_reg_ptr(USART_TypeDef *uartx, const char *reg)
{
    if(!mystrncmp(reg, "sr", 2))
        return &(uartx->SR);
    if(!mystrncmp(reg, "dr", 2))
        return &(uartx->DR);
    if(!mystrncmp(reg, "brr", 3))
        return &(uartx->BRR);
    if(!mystrncmp(reg, "cr1", 3))
        return &(uartx->CR1);
    if(!mystrncmp(reg, "cr2", 3))
        return &(uartx->CR2);
    if(!mystrncmp(reg, "cr3", 3))
        return &(uartx->CR3);
    if(!mystrncmp(reg, "gtpr", 4))
        return &(uartx->GTPR);
    return 0;

}

uint32_t uart_get_reg(USART_TypeDef *uartx, const char *reg)
{
    volatile uint32_t *reg_ptr = uart_get_reg_ptr(uartx, reg);
    if(reg_ptr)
        return *reg_ptr;
    else
        return -1;
}

uint32_t uart_set_reg(USART_TypeDef *uartx, const char *reg, uint32_t val)
{
    volatile uint32_t *reg_ptr = uart_get_reg_ptr(uartx, reg);
    if(reg_ptr)
    {
        *reg_ptr = val;
        return val;
    }
    else
        return -1;
}

