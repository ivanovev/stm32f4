
#include <main.h>
#include "gpio/led.h"

const uint8_t saw[16] = {0x0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0};
const uint8_t test[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t test2[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t sin32[32] = {128,152,176,198,217,233,245,252,255,252,245,233,217,198,176,152,128,104,80,58,39,23,11,4,1,4,11,23,39,58,80,104};
const uint8_t sin64[64] = {128,140,152,164,176,187,198,208,217,226,233,240,245,249,252,254,255,254,252,249,245,240,233,226,217,208,198,187,176,164,152,140,128,116,104,92,80,69,58,48,39,30,23,16,11,7,4,2,1,2,4,7,11,16,23,30,39,48,58,69,80,92,104,116};
const uint8_t sin84[84] = {128,137,146,156,165,174,183,191,199,207,214,221,227,232,237,242,246,249,251,253,254,255,254,253,251,249,246,242,237,232,227,221,214,207,199,191,183,174,165,156,146,137,128,119,110,100,91,82,73,65,57,49,42,35,29,24,19,14,10,7,5,3,2,1,2,3,5,7,10,14,19,24,29,35,42,49,57,65,73,82,91,100,110,119};
int main(void)
{
    myinit();
    dac_start(sin84, sizeof(sin84));
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}
