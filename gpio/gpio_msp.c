
#include <main.h>

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
#ifdef ENABLE_VFD
    vfd_gpio_exti_cb();
#endif
#ifdef BTN_PIN
    if(btn_state())
    {
        led_toggle();
    }
#endif
}

