
#include "uart.h"
#include "util/queue.h"

#pragma message "UART: UART" STR(UARTn)
#pragma message "UART_BAUDRATE: " STR(UART_BAUDRATE)

#define UART_RX_ENABLE 1

UART_HandleTypeDef huart;
Queue quart;

uint32_t uart_recv_str(char *buf)
{
    return dequeue(&quart, (uint8_t*)buf, 1);
}

#if UART_RX_ENABLE
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t *ptr = &quart.q[quart.head];
    if(ptr[0] != '\r')
        enqueue(&quart, ptr[0]);
    HAL_UART_Transmit(huart, ptr, 1, 0);
    HAL_UART_Receive_IT(huart, (uint8_t *)&quart.q[quart.head], 1);
}
#endif

extern void Error_Handler(void);

void uart_init(void)
{
    quart.head = 0;
    quart.tail = 0;
    huart.Instance        = UARTx;
    huart.Init.BaudRate   = UART_BAUDRATE;
    huart.Init.WordLength = UART_WORDLENGTH_8B;
    huart.Init.StopBits   = UART_STOPBITS_1;
    huart.Init.Parity     = UART_PARITY_NONE;
    huart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    huart.Init.Mode       = UART_MODE_TX_RX;

    if(HAL_UART_Init(&huart) != HAL_OK)
    {
        Error_Handler();
    }

#if UART_RX_ENABLE
    if(HAL_UART_Receive_IT(&huart, (uint8_t *)&quart.q[quart.head], 1) != HAL_OK)
    {
        Error_Handler();
    }
#endif
}

void uart_send_str(const char *str, uint32_t len)
{
    HAL_UART_Transmit(&huart, (uint8_t*)str, len, 100);
}

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
    uart_send_str2(str);
    uart_send_str(": ", 0);
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

