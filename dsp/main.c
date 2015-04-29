
#include <main.h>
#include "gpio/led.h"
#include "arm_math.h"

#define BLOCK_SIZE            32
#define NUM_TAPS              29

const float32_t firCoeffs32[NUM_TAPS] = {
    -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
    -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
    +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
    +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f
};

static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];

int main(void)
{
    myinit();
    arm_fir_instance_f32 S;
    arm_status status;
    arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], BLOCK_SIZE);
    for (;;) {
        HAL_Delay(1000);
        led_toggle();
    }
}

