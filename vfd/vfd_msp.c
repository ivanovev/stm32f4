
#include "vfd/vfd.h"

#pragma message "VFD_UART_TX: GPIO" STR(VFD_UART_TX_GPIO) " PIN" STR(VFD_UART_TX_PIN)

#pragma message "VFD_UART_AF: " STR(GPIO_AF_VFD_UARTx)

#if 0
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(VFD_UART_TX_GPIO, VFD_UART_TX_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF_VFD_UARTx);
    VFD_UARTx_CLK_ENABLE();
}
#endif

