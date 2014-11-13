
#include "util/system_msp.h"

extern void SystemClock_Config(void);
extern uint32_t SystemCoreClock;

void myinit(void)
{
#pragma message "myinit()"
#pragma message "HAL_Init()"
#pragma message "SystemClock_Config()"
    HAL_Init();
    SystemClock_Config();

#ifdef MY_GPIO
#pragma message "led_init()"
    led_init();
#ifdef btn_init
#pragma message "btn_init()"
    btn_init();
#endif
#endif

#ifdef MY_UART
#pragma message "uart_init()"
    uart_init();
#endif

#ifdef MY_I2C
#pragma message "i2c_init()"
    i2c_init();
#endif

#ifdef MY_FLASH
#pragma message "flash_erase_img1()"
    flash_erase_img1();
#endif

#ifdef MY_PCL
#pragma message "pcl_init()"
    pcl_init();
#endif

#ifdef MY_ETH
#pragma message "eth_init()"
    eth_init();
#endif

#ifdef MY_USB
#pragma message "usb_init()"
    usb_init();
#endif

#ifdef MY_TIM
#pragma message "tim_init()"
    tim_init();
#endif

#ifdef MY_VFD
#pragma message "vfd_init()"
    vfd_init();
#endif

}

void mydeinit(void)
{
#pragma message "mydeinit()"
#ifdef MY_TIM
#pragma message "tim_deinit()"
    tim_deinit();
#endif

#ifdef MY_ETH
#pragma message "eth_deinit()"
    eth_deinit();
#endif

#ifdef MY_VFD
#pragma message "vfd_deinit()"
    vfd_deinit();
#endif
}

