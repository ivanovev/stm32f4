
#include <main.h>

int main(void)
{
    myinit();
    //led_on();
    //vfd_cls();
    for (;;)
    {
        //HAL_Delay(2000);
        //vfd_str("1");
        vfd_upd();
    }
}

#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    led_toggle();
}
#endif

