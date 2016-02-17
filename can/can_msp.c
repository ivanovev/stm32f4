
#include <main.h>

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(CAN_TX_GPIO, CAN_TX_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF9_CAN1);
    GPIO_INIT(CAN_RX_GPIO, CAN_RX_PIN, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF9_CAN1);
    CAN_CLK_ENABLE();
    HAL_NVIC_SetPriority(CANx_RX_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(CANx_RX_IRQn);
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan)
{
    CAN_CLK_DISABLE();
}

