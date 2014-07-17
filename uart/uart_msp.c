
#include "uart.h"

#pragma message "UART_TX: GPIO" STR(UART_TX_GPIO) " PIN" STR(UART_TX_PIN)
#pragma message "UART_RX: GPIO" STR(UART_RX_GPIO) " PIN" STR(UART_RX_PIN)

#pragma message "UART_AF: " STR(GPIO_AF_UARTx)
#pragma message "UARTx_IRQn: " STR(UARTx_IRQn)

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init;
    UART_PINMUX(UARTn, UART_TX_GPIO, UART_TX_PIN, UART_RX_GPIO, UART_RX_PIN, GPIO_AF_UARTx);
    UARTx_CLK_ENABLE();

    HAL_NVIC_SetPriority(UARTx_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UARTx_IRQn);
}

