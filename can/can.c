
#include "can/can.h"

CAN_HandleTypeDef hcan;
extern void Error_Handler(void);
uint32_t rxcounter = 0;

void can_init(void)
{
    CAN_FilterConfTypeDef filtercfg;
    static CanTxMsgTypeDef cantxmsg;
    static CanRxMsgTypeDef canrxmsg;
    hcan.Instance = CANx;
    hcan.pTxMsg = &cantxmsg;
    hcan.pRxMsg = &canrxmsg;
    hcan.Init.TTCM = DISABLE;
    hcan.Init.ABOM = DISABLE;
    hcan.Init.AWUM = DISABLE;
    hcan.Init.NART = ENABLE;
    hcan.Init.RFLM = DISABLE;
    hcan.Init.TXFP = DISABLE;
    hcan.Init.Mode = CAN_MODE_LOOPBACK;
    hcan.Init.SJW = CAN_SJW_1TQ;
    hcan.Init.BS1 = CAN_BS1_3TQ;
    hcan.Init.BS2 = CAN_BS2_2TQ;
    hcan.Init.Prescaler = 7;
    if(HAL_CAN_Init(&hcan) != HAL_OK)
    {
        Error_Handler();
    }

    filtercfg.FilterNumber = 0;
    filtercfg.FilterMode = CAN_FILTERMODE_IDMASK;
    filtercfg.FilterScale = CAN_FILTERSCALE_32BIT;
    filtercfg.FilterIdHigh = 0x0000;
    filtercfg.FilterIdLow = 0x0000;
    filtercfg.FilterMaskIdHigh = 0x0000;
    filtercfg.FilterMaskIdLow = 0x0000;
    filtercfg.FilterFIFOAssignment = 0;
    filtercfg.FilterActivation = ENABLE;
    filtercfg.BankNumber = 14;
    if(HAL_CAN_ConfigFilter(&hcan, &filtercfg) != HAL_OK)
    {
        Error_Handler();
    }

    hcan.pTxMsg->StdId = 0x321;
    hcan.pTxMsg->ExtId = 0x01;
    hcan.pTxMsg->RTR = CAN_RTR_DATA;
    hcan.pTxMsg->IDE = CAN_ID_STD;
    hcan.pTxMsg->DLC = 8;

#if 1
    if(HAL_CAN_Receive_IT(&hcan, CAN_FIFO0) != HAL_OK)
    {
        Error_Handler();
    }
#endif
}

void can_deinit()
{
    HAL_CAN_DeInit(&hcan);
}

uint32_t can_send_data(uint8_t data[])
{
    uint32_t i, j;
    for(i = 0; i < 8; i++)
        hcan.pTxMsg->Data[i] = data ? data[i] : (uint8_t)i;
    for(i = 0; i < 1; i++)
    {
        j = HAL_CAN_Transmit_IT(&hcan);
        if(j != HAL_OK)
        {
            break;
            //Error_Handler();
        }
    }
    return j;
}

volatile uint32_t* can_get_reg_ptr(char *reg)
{
    if(!mystrncmp(reg, "mcr", 3))
        return &(hcan.Instance->MCR);
    if(!mystrncmp(reg, "msr", 3))
        return &(hcan.Instance->MSR);
    if(!mystrncmp(reg, "tsr", 3))
        return &(hcan.Instance->TSR);
    if(!mystrncmp(reg, "esr", 3))
        return &(hcan.Instance->ESR);
    if(!mystrncmp(reg, "btr", 3))
        return &(hcan.Instance->BTR);
    return 0;
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *phcan)
{
    led_toggle();
    if((phcan->pRxMsg->StdId == 0x321) && (phcan->pRxMsg->IDE == CAN_ID_STD))
    {
        rxcounter += phcan->pRxMsg->DLC;
    }

#if 1
    HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
#endif
}

