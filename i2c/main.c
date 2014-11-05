
#include <main.h>
#include "i2c/eeprom.h"

int main(void)
{
    myinit();
    //led_on();
    //eeprom_ipaddr_write(0x01020304);
    uint32_t ipaddr = eeprom_ipaddr_read();
    for (;;)
    {
        i2c_send1();
    }
}

