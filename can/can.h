
#ifndef __CAN_H__
#define __CAN_H__

#include <main.h>

#ifndef CANn
#define CANn 1
#endif

#ifdef CANn
#if CANn == 1
#define CANx                JOIN(CAN, CANn)
#define CAN_CLK_ENABLE      JOIN3(__HAL_RCC_CAN, CANn, _CLK_ENABLE)
#define CAN_CLK_DISABLE     JOIN3(__HAL_RCC_CAN, CANn, _CLK_DISABLE)
#define CANx_RX_IRQn        JOIN3(CAN, CANn, _RX0_IRQn)
#define CANx_RX_IRQHandler  JOIN3(CAN, CANn, _RX0_IRQHandler)
#define CANx_TX_IRQn        JOIN3(CAN, CANn, _TX_IRQn)
#define CANx_TX_IRQHandler  JOIN3(CAN, CANn, _TX_IRQHandler)
#define CAN_TX_GPIO B
#define CAN_RX_GPIO B
#define CAN_TX_PIN 9
#define CAN_RX_PIN 8

#define CAN_TX1_GPIO D
#define CAN_RX1_GPIO D
#define CAN_TX1_PIN 0
#define CAN_RX1_PIN 1

#endif
#endif

void                can_init(void);
void                can_deinit(void);
uint32_t            can_send_data(uint8_t data[]);
volatile uint32_t*  can_get_reg_ptr(char *reg);

#endif

