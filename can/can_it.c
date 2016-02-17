
#include "can.h"

#ifdef CANn
#pragma message "CAN_IRQHanler: " STR(CANx_RX_IRQHandler)

extern CAN_HandleTypeDef hcan;

void CANx_RX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan);
}
#endif

