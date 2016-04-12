
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <main.h>

#define DISPLAY_EVT_BTNL    (1 << 0)
#define DISPLAY_EVT_BTNR    (1 << 1)
#define DISPLAY_EVT_BTNU    (1 << 2)
#define DISPLAY_EVT_BTND    (1 << 3)
#define DISPLAY_EVT_BTNO    (1 << 4)
#define DISPLAY_EVT_TIM_UPD (1 << 5)

#define DISP_TIMn            7
#define DISP_TIMx            JOIN(TIM, DISP_TIMn)
#define DISP_TIMx_INTERVAL   1000

#define DISP_TIMx_CLK_ENABLE     JOIN3(__TIM, DISP_TIMn, _CLK_ENABLE)
#define DISP_TIMx_IRQn           JOIN3(TIM, DISP_TIMn, _IRQn)
#define DISP_TIMx_FORCE_RESET JOIN3(__TIM, DISP_TIMn, _FORCE_RESET)
#define DISP_TIMx_RELEASE_RESET JOIN3(__TIM, DISP_TIMn, _RELEASE_RESET)
#define DISP_TIMx_IRQHandler     JOIN3(TIM, DISP_TIMn, _IRQHandler)

#define BTNR_GPIO E
#define BTNR_PIN 0
#define BTNR_IRQn   EXTI0_IRQn

#define BTNO_GPIO E
#define BTNO_PIN 2
#define BTNO_IRQn   EXTI2_IRQn

#define BTNL_GPIO E
#define BTNL_PIN 3
#define BTNL_IRQn   EXTI3_IRQn

#define BTND_GPIO E
#define BTND_PIN 4
#define BTND_IRQn   EXTI4_IRQn

#define BTNU_GPIO D
#define BTNU_PIN 7
#define BTNU_IRQn EXTI9_5_IRQn

void        display_init(void);
void        display_deinit(void);
void        display_reset(void);
void        display_crlf(void);
void        display_cls(void);
void        display_home(void);

void        display_str(const char *str);

#endif

