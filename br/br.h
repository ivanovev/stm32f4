
#ifndef __BR_H__
#define __BR_H__

#include <main.h>

#if ALT_UARTn == 4 || ALT_UARTn == 5
#define ALT_UARTx_type UART
#else
#define ALT_UARTx_type USART
#endif

#define ALT_UARTx JOIN(ALT_UARTx_type, ALT_UARTn)
#define ALT_UARTx_CLK_ENABLE JOIN4(__, ALT_UARTx_type, ALT_UARTn, _CLK_ENABLE)

#if ALT_UARTn == 1 || ALT_UARTn == 2 || ALT_UARTn == 3
#define GPIO_AF_ALT_UARTx JOIN3(GPIO_AF7_, ALT_UARTx_type, ALT_UARTn)
#endif
#if ALT_UARTn == 4 || ALT_UARTn == 5 || ALT_UARTn == 6
#define GPIO_AF_ALT_UARTx JOIN(GPIO_AF8_, ALT_UARTx_type, ALT_UARTn)
#endif

void br_init(void);

#endif
