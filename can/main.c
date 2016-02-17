
#include <main.h>

int main(void)
{
    myinit();
    //uint32_t rv = rng_uint32();
    can_send_data(0);
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

