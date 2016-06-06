
#include <main.h>
#include "i2c/eeprom.h"

int main(void)
{
    myinit();
    uint8_t buf[2] = {0x12, 0x34};
    eeprom_read_data(0, buf, 1);
    //led_on();
    //eeprom_ipaddr_write(0x01020304);
#if 0
    uint8_t ipaddr8[4];
    uint32_t ipaddr = eeprom_ipaddr_read(ipaddr8);
#endif
    for (;;)
    {
        led_toggle();
        i2c_send(0xAE, (uint8_t*)buf, sizeof(buf));
        HAL_Delay(1000);
    }
}

