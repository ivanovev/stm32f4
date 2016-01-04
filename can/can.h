
#ifndef __CAN_H__
#define __CAN_H__

#include <main.h>

#ifndef CANn
#define CANn 1
#endif

#ifdef CANn
#if CANn == 1
#define CANx JOIN(CAN, CANn)
#define CAN_CLK_ENABLE JOIN3(__HAL_RCC_CAN, CANn, _CLK_ENABLE)
#define CAN_CLK_DISABLE JOIN3(__HAL_RCC_CAN, CANn, _CLK_DISABLE)
#define CAN_TX_GPIO D
#define CAN_RX_GPIO D
#define CAN_TX_PIN 1
#define CAN_RX_PIN 0
#endif
#endif

void                can_init(void);
void                can_deinit(void);
uint32_t            can_send_data(uint8_t data[]);
volatile uint32_t*  can_get_reg_ptr(char *reg);

#endif

