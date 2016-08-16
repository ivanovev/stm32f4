
#include "uart.h"

#ifdef UARTn
#pragma message "IRQHanler: " STR(UARTx_IRQHandler)

extern UART_HandleTypeDef huart;

#ifndef UART_RX_DISABLE
void UARTx_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart);
}
#endif
#endif

