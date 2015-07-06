
#include "util/system_msp.h"
#include "gpio/led.h"

extern void SystemClock_Config(void);
extern uint32_t SystemCoreClock;

void myinit(void)
{
#pragma message "myinit()"
#pragma message "HAL_Init()"
#pragma message "SystemClock_Config()"
    HAL_Init();
    SystemClock_Config();

#ifdef ENABLE_GPIO
#pragma message "led_init()"
    led_init();
#ifdef btn_init
#pragma message "btn_init()"
    btn_init();
#endif
#endif

#ifdef ENABLE_RNG
#pragma message "rng_init()"
    rng_init();
#endif

#ifdef ENABLE_UART
#pragma message "uart_init()"
    uart_init();
#endif

#ifdef ENABLE_SDIO
#pragma message "sdio_init()"
    sdio_init();
#endif

#ifdef ENABLE_I2C
#pragma message "i2c_init()"
    i2c_init();
#endif

#ifdef ENABLE_TIM
#pragma message "tim_init()"
    tim_init();
#endif

#ifdef ENABLE_ADC
#pragma message "adc_init()"
    adc_init();
#endif

#ifdef ENABLE_DAC
#pragma message "dac_init()"
    dac_init();
#endif

#ifdef ENABLE_DMA
#pragma message "dma_init()"
    dma_init();
#endif

#ifdef ENABLE_FLASH
#pragma message "flash_erase_img1()"
    flash_erase_img1();
#endif

#ifdef ENABLE_ETH
#pragma message "eth_init()"
    eth_init();
#endif

#ifdef ENABLE_DSP
#pragma message "dsp_init()"
    dsp_init();
#endif

#ifdef ENABLE_USB
#pragma message "usb_init()"
    usb_init();
#endif

#ifdef ENABLE_PCL
#pragma message "pcl_init()"
    pcl_init();
#endif

#ifdef ENABLE_VFD
#pragma message "vfd_init()"
    vfd_init();
#endif

#ifdef ENABLE_BR
#pragma message "br_init()"
    br_init();
#endif
}

void mydeinit(void)
{
#pragma message "mydeinit()"
#ifdef ENABLE_TIM
#pragma message "tim_deinit()"
    tim_deinit();
#endif

#ifdef ENABLE_ETH
#pragma message "eth_deinit()"
    eth_deinit();
#endif

#ifdef ENABLE_VFD
#pragma message "vfd_deinit()"
    vfd_deinit();
#endif

#ifdef ENABLE_RNG
#pragma message "rng_deinit()"
    rng_deinit();
#endif
}

uint32_t uptime(void)
{
    return HAL_GetTick()/1000;
}

