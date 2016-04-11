
#include <main.h>
#include "i2c/eeprom.h"

int main(void)
{
    myinit();
    //led_on();
    //eeprom_ipaddr_write(0x01020304);
#if 0
    uint8_t ipaddr8[4];
    uint32_t ipaddr = eeprom_ipaddr_read(ipaddr8);
#endif
    char buf[2] = {0x12, 0x34};
    for (;;)
    {
        i2c_send(0xAE, (uint8_t*)buf, sizeof(buf));
        HAL_Delay(1000);
    }
}

