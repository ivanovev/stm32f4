
#include <main.h>
#include "vfd_menu.h"

#pragma message "VFD_UART: UART" STR(VFD_UARTn)
#pragma message "VFD_UART_BAUDRATE: " STR(VFD_UART_BAUDRATE)

UART_HandleTypeDef hvfduart;

extern void Error_Handler(void);

static volatile uint32_t vfd_state = 0;

void vfd_init(void)
{
    GPIO_InitTypeDef gpio_init;
    GPIO_INIT(VFD_UART_TX_GPIO, VFD_UART_TX_PIN, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FAST, GPIO_AF_VFD_UARTx);
    VFD_UARTx_CLK_ENABLE();

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

    //GPIO_InitTypeDef gpio_init;
    GPIO_INIT(VFD_BUSY_GPIO, VFD_BUSY_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    //GPIO_INIT(VFD_RESETN_GPIO, VFD_RESETN_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW, 0);

    GPIO_INIT(BTNL_GPIO, BTNL_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    GPIO_INIT(BTNR_GPIO, BTNR_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    GPIO_INIT(BTNU_GPIO, BTNU_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    GPIO_INIT(BTND_GPIO, BTND_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);
    GPIO_INIT(BTNO_GPIO, BTNO_PIN, GPIO_MODE_IT_FALLING, GPIO_NOPULL, GPIO_SPEED_LOW, 0);

    HAL_NVIC_SetPriority(BTNL_IRQn, 0xF, 0);
    HAL_NVIC_EnableIRQ(BTNL_IRQn);
    HAL_NVIC_SetPriority(BTNR_IRQn, 0xF, 0);
    HAL_NVIC_EnableIRQ(BTNR_IRQn);
    HAL_NVIC_SetPriority(BTNU_IRQn, 0xF, 0);
    HAL_NVIC_EnableIRQ(BTNU_IRQn);
    HAL_NVIC_SetPriority(BTND_IRQn, 0xF, 0);
    HAL_NVIC_EnableIRQ(BTND_IRQn);
    HAL_NVIC_SetPriority(BTNO_IRQn, 0xF, 0);
    HAL_NVIC_EnableIRQ(BTNO_IRQn);

    //vfd_reset();
    vfd_menu_init();
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

uint16_t vfd_btn_status(void)
{
    uint16_t status = 0;
    if(HAL_GPIO_ReadPin(GPIO(BTNL_GPIO), PIN(BTNL_PIN)) == 0)
        status |= BTNL_STATUS;
    if(HAL_GPIO_ReadPin(GPIO(BTNR_GPIO), PIN(BTNR_PIN)) == 0)
        status |= BTNR_STATUS;
    if(HAL_GPIO_ReadPin(GPIO(BTNU_GPIO), PIN(BTNU_PIN)) == 0)
        status |= BTNU_STATUS;
    if(HAL_GPIO_ReadPin(GPIO(BTND_GPIO), PIN(BTND_PIN)) == 0)
        status |= BTND_STATUS;
    if(HAL_GPIO_ReadPin(GPIO(BTNO_GPIO), PIN(BTNO_PIN)) == 0)
        status |= BTNO_STATUS;
    return status;
}

void vfd_send_str(const char *str, uint16_t len)
{
    //uint16_t len = mystrnlen(str,  MAXSTR);
    vfd_wait();
    HAL_UART_Transmit(&hvfduart, (uint8_t*)str, len, 100);
}

void vfd_str(const char *str)
{
    int j;
#if 0
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
        vfd_send_str(tx, len);
        return;
    }
    vfd_send_str(str, mystrnlen(str, 32));
}

void vfd_cls(void)
{
    vfd_str("0x0C");
}

void vfd_cp866(void)
{
    vfd_str("0x1B7411");
}

uint8_t vfd_brightness(int8_t newlvl)
{
    static uint8_t lvl = 3;
    if((1 <= newlvl) && (newlvl <= 8))
        lvl = newlvl;
    else
        return lvl;
    char buf[16];
    mysnprintf(buf, sizeof(buf), "0x1F58%02X", lvl);
    vfd_str(buf);
    return lvl;
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
#if 1
    uint16_t btn_status = vfd_btn_status();
    if(btn_status)
    {
        vfd_state = btn_status;
        led_toggle();
    }
#endif
}

void vfd_upd(void)
{
    if(vfd_state)
    {
        if(vfd_state & BTNU_STATUS)
            vfd_menu_up();
        if(vfd_state & BTND_STATUS)
            vfd_menu_down();
        if(vfd_state & BTNL_STATUS)
            vfd_menu_left();
        if(vfd_state & BTNR_STATUS)
            vfd_menu_right();
        //led_toggle();
        vfd_state = 0;
    }
}

