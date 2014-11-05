
#ifndef __VFD_H__
#define __VFD_H__

#include <main.h>

#define VFD_UARTn 4
#define VFD_UART_TX_GPIO A
#define VFD_UART_TX_PIN 0

#define VFD_UART_BAUDRATE 38400

#if VFD_UARTn == 4 || VFD_UARTn == 5
#define VFD_UARTx_type UART
#define VFD_UARTx_CLK_ENABLE JOIN3(__UART, VFD_UARTn, _CLK_ENABLE)
#else
#define VFD_UARTx_type USART
#define VFD_UARTx_CLK_ENABLE JOIN3(__USART, VFD_UARTn, _CLK_ENABLE)
#endif

#define VFD_UARTx JOIN(VFD_UARTx_type, VFD_UARTn)
#define VFD_UARTx_IRQn JOIN3(VFD_UARTx_type, VFD_UARTn, _IRQn)
#define VFD_UARTx_IRQHandler JOIN3(VFD_UARTx_type, VFD_UARTn, _IRQHandler)

#if VFD_UARTn == 1 || VFD_UARTn == 2 || VFD_UARTn == 3
#define GPIO_AF_VFD_UARTx JOIN(GPIO_AF7_USART, VFD_UARTn)
#endif
#if VFD_UARTn == 4 || VFD_UARTn == 5
#define GPIO_AF_VFD_UARTx JOIN(GPIO_AF8_UART, VFD_UARTn)
#endif
#if VFD_UARTn == 6
#define GPIO_AF_VFD_UARTx JOIN(GPIO_AF8_USART, VFD_UARTn)
#endif

#define VFD_BUSY_GPIO E
#define VFD_BUSY_PIN 1

#define VFD_RESETN_GPIO D
#define VFD_RESETN_PIN 4

#define BTNR_GPIO E
#define BTNR_PIN 0
#define BTNR_IRQn   EXTI0_IRQn

#define BTNO_GPIO E
#define BTNO_PIN 2
#define BTNO_IRQn   EXTI2_IRQn

#define BTNL_GPIO E
#define BTNL_PIN 3
#define BTNL_IRQn   EXTI3_IRQn

#define BTND_GPIO E
#define BTND_PIN 4
#define BTND_IRQn   EXTI4_IRQn

#define BTNU_GPIO D
#define BTNU_PIN 7
#define BTNU_IRQn EXTI9_5_IRQn

#define BTNL_STATUS (1 << 0)
#define BTNR_STATUS (1 << 1)
#define BTNU_STATUS (1 << 2)
#define BTND_STATUS (1 << 3)
#define BTNO_STATUS (1 << 4)

void vfd_init(void);
void vfd_reset(void);
void vfd_upd(void);
uint16_t vfd_btn_status(void);
void vfd_send_str(const char *str, uint16_t len);
void vfd_str(const char *str);
void vfd_cls(void);
void vfd_cp866(void);
uint8_t vfd_brightness(int8_t newlvl);

#endif

