
#include <main.h>
#include "vfd_menu.h"
#include "gpio/btn.h"

#pragma message "VFD_UART: UART" STR(VFD_UARTn)
#pragma message "VFD_UART_BAUDRATE: " STR(VFD_UART_BAUDRATE)
#pragma message "VFD_UART_TX: GPIO" STR(VFD_UART_TX_GPIO) " PIN" STR(VFD_UART_TX_PIN)
#pragma message "VFD_UART_AF: " STR(GPIO_AF_VFD_UARTx)

UART_HandleTypeDef hvfduart;
TIM_HandleTypeDef hvfdtim;

extern void Error_Handler(void);

void vfd_init(void)
{
    hvfduart.Instance        = VFD_UARTx;
    hvfduart.Init.BaudRate   = VFD_UART_BAUDRATE;
    hvfduart.Init.WordLength = UART_WORDLENGTH_8B;
    hvfduart.Init.StopBits   = UART_STOPBITS_1;
    hvfduart.Init.Parity     = UART_PARITY_NONE;
    hvfduart.Init.Mode       = UART_MODE_TX;
    hvfduart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;

    if(HAL_UART_Init(&hvfduart) != HAL_OK)
    {
        led_on();
    }

    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(VFD_BUSY_GPIO, VFD_BUSY_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
}

void vfd_deinit(void)
{
    HAL_TIM_Base_Stop_IT(&hvfdtim);
    HAL_TIM_Base_DeInit(&hvfdtim);
}

void VFD_TIMx_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&hvfdtim);
}

uint16_t vfd_busy(void)
{
    return HAL_GPIO_ReadPin(GPIO(VFD_BUSY_GPIO), PIN(VFD_BUSY_PIN));
}

void vfd_wait(void)
{
    uint16_t i;
    for(i = 0; i < 20; i++)
    {
        if(!vfd_busy())
            break;
        HAL_Delay(5);
    }
}

void vfd_reset(void)
{
    HAL_GPIO_WritePin(GPIO(VFD_RESETN_GPIO), PIN(VFD_RESETN_PIN), GPIO_PIN_RESET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIO(VFD_RESETN_GPIO), PIN(VFD_RESETN_PIN), GPIO_PIN_SET);
    vfd_wait();
}

uint16_t vfd_btn_state(void)
{
    uint16_t state = 0;
    if(HAL_GPIO_ReadPin(GPIO(BTNL_GPIO), PIN(BTNL_PIN)) == 0)
        state |= VFD_EVT_BTNL;
    if(HAL_GPIO_ReadPin(GPIO(BTNR_GPIO), PIN(BTNR_PIN)) == 0)
        state |= VFD_EVT_BTNR;
    if(HAL_GPIO_ReadPin(GPIO(BTNU_GPIO), PIN(BTNU_PIN)) == 0)
        state |= VFD_EVT_BTNU;
    if(HAL_GPIO_ReadPin(GPIO(BTND_GPIO), PIN(BTND_PIN)) == 0)
        state |= VFD_EVT_BTND;
    if(HAL_GPIO_ReadPin(GPIO(BTNO_GPIO), PIN(BTNO_PIN)) == 0)
        state |= VFD_EVT_BTNO;
    return state;
}

void vfd_send_str(const char *str, uint16_t len)
{
    //uint16_t len = mystrnlen(str,  MAXSTR);
    vfd_wait();
    HAL_UART_Transmit(&hvfduart, (uint8_t*)str, len, 100);
}

void vfd_str(const char *str)
{
#if 0
    int j;
    if(pwords)
    {
        for(j = 0; j < pwords->len/2; j++)
        {
            if(!strcmp(str, pwords->en_ru[2*j]))
            {
                str = pwords->en_ru[2*j+1];
                break;
            }
        }
    }
#endif
    uint8_t tx[32];
    uint16_t len = 0;
    if(!mystrncmp(str, "0x", 2))
    {
        len = str2bytes(str, tx, sizeof(tx));
        vfd_send_str((char*)tx, len);
        return;
    }
    vfd_send_str(str, mystrnlen(str, 32));
}

void vfd_cp866(void)
{
    vfd_str("0x1B7411");
}

void vfd_home(void)
{
    vfd_str("0x0B");
}

void vfd_cls(void)
{
    vfd_str("0x0C");
}

void vfd_crlf(void)
{
    vfd_str("0x0D");
    vfd_str("0x0A");
}

uint8_t vfd_brightness(int8_t newlvl)
{
    static uint8_t lvl = 1;
    if((1 <= newlvl) && (newlvl <= 8))
        lvl = newlvl;
    else
        return lvl;
    char buf[16];
    mysnprintf(buf, sizeof(buf), "0x1F58%02X", lvl);
    vfd_str(buf);
    return lvl;
}

