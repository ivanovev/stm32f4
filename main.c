
#include <main.h>

int main(void)
{
    myinit();
    for(;;)
    {
        HAL_Delay(3000);
        led_toggle();
    }
    return 0;
}
