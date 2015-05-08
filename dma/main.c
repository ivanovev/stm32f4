
#include <main.h>
#include "gpio/led.h"

__ALIGN_BEGIN uint8_t buf1[IO_BUF_SZ] __ALIGN_END;
__ALIGN_BEGIN uint8_t buf2[IO_BUF_SZ] __ALIGN_END;

int main(void)
{
    myinit();
    mymemset((void*)buf1, 1, IO_BUF_SZ);
    dma_memcpy((void*)buf2, (void*)buf1, IO_BUF_SZ);
    for (;;) {
        HAL_Delay(1000);
        //led_toggle();
    }
}

