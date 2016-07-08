
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <main.h>

#ifdef ENABLE_LCD
#include "display/lcd/lcd.h"
#endif
#ifdef ENABLE_VFD
#include "display/vfd/vfd.h"
#endif

#define EVT_BTNL    (1 << 0)
#define EVT_BTNR    (1 << 1)
#define EVT_BTNU    (1 << 2)
#define EVT_BTND    (1 << 3)
#define EVT_BTNO    (1 << 4)
#define EVT_TIM_UPD (1 << 5)

#define DISP_TIMn            7
#define DISP_TIMx            JOIN(TIM, DISP_TIMn)
#define DISP_TIMx_INTERVAL   1000

#define DISP_TIMx_CLK_ENABLE     JOIN3(__TIM, DISP_TIMn, _CLK_ENABLE)
#define DISP_TIMx_IRQn           JOIN3(TIM, DISP_TIMn, _IRQn)
#define DISP_TIMx_FORCE_RESET JOIN3(__TIM, DISP_TIMn, _FORCE_RESET)
#define DISP_TIMx_RELEASE_RESET JOIN3(__TIM, DISP_TIMn, _RELEASE_RESET)
#define DISP_TIMx_IRQHandler     JOIN3(TIM, DISP_TIMn, _IRQHandler)

void        display_init(void);
void        display_init_tim(void);
void        display_deinit(void);
void        display_deinit_tim(void);
void        display_reset(void);
void        display_crlf(void);
void        display_cls(void);
void        display_home(void);
void        display_upd(void);
void        display_tim_upd(void);
uint16_t    display_gpio_exti_cb(void);

void        display_str(const char *str);

#endif

