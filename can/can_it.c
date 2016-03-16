
#include "can.h"

#ifdef CANn
#pragma message "CAN_IRQHanler: " STR(CANx_RX_IRQHandler)

extern CAN_HandleTypeDef hcan;

void CANx_RX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan);
}

void CANx_TX_IRQHandler(void)
{
#if 0
    if(CAN_GetITStatus(CANx,CAN_IT_TME))
    {
        CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
    }
#endif
}
#endif

