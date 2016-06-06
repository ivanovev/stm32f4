
#include <main.h>

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
#ifdef ENABLE_DISPLAY
    display_gpio_exti_cb();
#endif
#ifdef ENABLE_PCL
    pcl_gpio_exti_cb();
#endif
#ifdef BTN_PIN
    if(btn_state())
    {
        led_toggle();
    }
#endif
}

