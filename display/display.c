
#include <main.h>
#include "display/display.h"

#ifdef ENABLE_VFD
#include "display/vfd/vfd.h"
#endif

#ifdef ENABLE_LCD
#include "display/lcd/lcd.h"
#endif

#ifdef ENABLE_VFD
#ifdef ENABLE_LCD
#error ENABLE either VFD or LCD
#endif
#endif

static volatile uint32_t display_state = 0;
TIM_HandleTypeDef hdisptim;

void display_init(void)
{
#ifdef ENABLE_VFD
    vfd_init();
#endif
#ifdef ENABLE_LCD
    lcd_init();
#endif
    GPIO_InitTypeDef gpio_init;

#if 1
    btn_irq_init(GPIO(BTNL_GPIO), BTNL_PIN);
    btn_irq_init(GPIO(BTNR_GPIO), BTNR_PIN);
    btn_irq_init(GPIO(BTNU_GPIO), BTNU_PIN);
    btn_irq_init(GPIO(BTND_GPIO), BTND_PIN);
    btn_irq_init(GPIO(BTNO_GPIO), BTNO_PIN);
#endif

    menu_init();

    hdisptim.Instance = DISP_TIMx;
    hdisptim.Init.Period = 10*DISP_TIMx_INTERVAL - 1;
    hdisptim.Init.Prescaler = tim_get_prescaler(DISP_TIMx);
    hdisptim.Init.ClockDivision = 0;
    hdisptim.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&hdisptim) != HAL_OK)
    {
        Error_Handler();
    }

    if(HAL_TIM_Base_Start_IT(&hdisptim) != HAL_OK)
    {
        Error_Handler();
    }
}

void display_deinit(void)
{
#ifdef ENABLE_VFD
    vfd_deinit();
#endif
#ifdef ENABLE_LCD
    lcd_deinit();
#endif
}

void DISP_TIMx_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&hdisptim);
}

void display_reset(void)
{
}

uint16_t display_btn_state(void)
{
    uint16_t state = 0;
    if(HAL_GPIO_ReadPin(GPIO(BTNL_GPIO), PIN(BTNL_PIN)) == 0)
        state |= EVT_BTNL;
    if(HAL_GPIO_ReadPin(GPIO(BTNR_GPIO), PIN(BTNR_PIN)) == 0)
        state |= EVT_BTNR;
    if(HAL_GPIO_ReadPin(GPIO(BTNU_GPIO), PIN(BTNU_PIN)) == 0)
        state |= EVT_BTNU;
    if(HAL_GPIO_ReadPin(GPIO(BTND_GPIO), PIN(BTND_PIN)) == 0)
        state |= EVT_BTND;
    if(HAL_GPIO_ReadPin(GPIO(BTNO_GPIO), PIN(BTNO_PIN)) == 0)
        state |= EVT_BTNO;
    return state;
}

void display_bytes(const uint8_t *bytes, uint16_t sz)
{
#ifdef ENABLE_VFD
    vfd_bytes(bytes, sz);
#endif
#ifdef ENABLE_LCD
    lcd_bytes(bytes, sz);
#endif
}

void display_str(const char *str)
{
    uint8_t tx[32];
    uint16_t len = 0;
    if(!mystrncmp(str, "0x", 2))
    {
        len = str2bytes(str, tx, sizeof(tx));
        display_bytes((char*)tx, len);
        return;
    }
    display_bytes(str, mystrnlen(str, 32));
}

void display_home(void)
{
#ifdef ENABLE_VFD
    vfd_home();
#endif
#ifdef ENABLE_LCD
    lcd_home();
#endif
}

void display_cls(void)
{
#ifdef ENABLE_VFD
    vfd_cls();
#endif
#ifdef ENABLE_LCD
    lcd_cls();
#endif
}

void display_crlf(void)
{
#ifdef ENABLE_VFD
    vfd_crlf();
#endif
#ifdef ENABLE_LCD
    lcd_crlf();
#endif
}

uint16_t display_gpio_exti_cb(void)
{
#if 1
    uint16_t btn_state = display_btn_state();
    dbg_send_hex2("btn_state", btn_state);
    if(btn_state)
    {
        led_toggle();
        display_state = btn_state;
        return display_state;
    }
#endif
    return 0;
}

void display_upd(void)
{
    if(display_state)
    {
        if(display_state & EVT_BTNU)
            menu_up();
        if(display_state & EVT_BTND)
            menu_down();
        if(display_state & EVT_BTNL)
            menu_left();
        if(display_state & EVT_BTNR)
            menu_right();
        if(display_state & EVT_BTNO)
            menu_ok();
        if(display_state & EVT_TIM_UPD)
            menu_tim_upd();
        //led_toggle();
        display_state = 0;
    }
}

void display_tim_upd(void)
{
    display_state |= EVT_TIM_UPD;
}

