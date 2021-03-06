
#include <main.h>

#ifdef UARTn
#pragma message "UART_TX: GPIO" STR(UART_TX_GPIO) " PIN" STR(UART_TX_PIN)
#pragma message "UART_RX: GPIO" STR(UART_RX_GPIO) " PIN" STR(UART_RX_PIN)

#pragma message "UART_AF: " STR(GPIO_AF_UARTx)
#pragma message "UARTx_IRQn: " STR(UARTx_IRQn)
#endif

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init;
#ifdef UARTn
    if(huart->Instance == UARTx)
    {
        UART_PINMUX(UART_TX_GPIO, UART_TX_PIN, UART_RX_GPIO, UART_RX_PIN, GPIO_AF_UARTx);
        UARTx_CLK_ENABLE();
#ifndef UART_RX_DISABLE
        HAL_NVIC_SetPriority(UARTx_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(UARTx_IRQn);
#endif
    }
#endif
#ifdef ENABLE_VFD
    if(huart->Instance == VFD_UARTx)
    {
        GPIO_INIT(VFD_UART_TX_GPIO, VFD_UART_TX_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF_VFD_UARTx);
        VFD_UARTx_CLK_ENABLE();
    }
#endif
#ifdef ENABLE_BR
    if(huart->Instance == ALT_UARTx)
    {
        UART_PINMUX(ALT_UART_TX_GPIO, ALT_UART_TX_PIN, ALT_UART_RX_GPIO, ALT_UART_RX_PIN, GPIO_AF_ALT_UARTx);
        ALT_UARTx_CLK_ENABLE();
    }
#endif
}

