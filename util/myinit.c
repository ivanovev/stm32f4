
#include "myinit.h"

extern void SystemClock_Config(void);
extern uint32_t SystemCoreClock;

void myinit(void)
{
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

#ifdef MY_PCL
#pragma message "pcl_init()"
    pcl_init();
#endif

#ifdef MY_UART
#pragma message "uart_init()"
    uart_init();
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

}

