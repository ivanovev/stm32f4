
#include <main.h>
#include "gpio/led.h"

static const uint8_t saw[16] = {0x0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0};
static const uint8_t test[16] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t test2[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t sin32[32] = {128,152,176,198,217,233,245,252,255,252,245,233,217,198,176,152,128,104,80,58,39,23,11,4,1,4,11,23,39,58,80,104};
static const uint8_t sin64[64] = {128,140,152,164,176,187,198,208,217,226,233,240,245,249,252,254,255,254,252,249,245,240,233,226,217,208,198,187,176,164,152,140,128,116,104,92,80,69,58,48,39,30,23,16,11,7,4,2,1,2,4,7,11,16,23,30,39,48,58,69,80,92,104,116};
//static const uint8_t sin84[84] = {128,137,146,156,165,174,183,191,199,207,214,221,227,232,237,242,246,249,251,253,254,255,254,253,251,249,246,242,237,232,227,221,214,207,199,191,183,174,165,156,146,137,128,119,110,100,91,82,73,65,57,49,42,35,29,24,19,14,10,7,5,3,2,1,2,3,5,7,10,14,19,24,29,35,42,49,57,65,73,82,91,100,110,119};
static const uint8_t sin84[84] = {128,136,144,152,161,168,176,184,191,197,204,210,215,220,224,228,232,235,237,238,239,240,239,238,237,235,232,228,224,220,215,210,204,197,191,183,176,168,161,152,144,136,128,120,112,104,95,88,80,72,65,59,52,46,41,36,32,28,24,21,19,18,17,16,17,18,19,21,24,28,32,36,41,46,52,59,65,73,80,88,95,104,112,120};

int main(void)
{
    myinit();
    dac_start(sin84, sizeof(sin84));
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

