
#include <main.h>

int main(void)
{
    myinit();
    uint32_t rv = rng_uint32();
    for (;;) {
        HAL_Delay(1000);
    }
}

