
#include <main.h>

UART_HandleTypeDef alt_huart;
#ifdef ENABLE_PCL
extern struct picolInterp *pcl_interp;
#endif

void br_init()
{
    uart_get_handle(&alt_huart, ALT_UARTn, HAL_UART_STATE_RESET);
    alt_huart.Init.BaudRate = ALT_UART_BAUDRATE;
    if(HAL_UART_Init(&alt_huart) != HAL_OK)
    {
        Error_Handler();
    }
#ifdef ENABLE_PCL
    pcl_br_init(pcl_interp);
#endif
}

