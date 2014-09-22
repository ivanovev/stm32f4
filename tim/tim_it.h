
#ifndef __TIM_IT_H__
#define __TIM_IT_H__

#include <main.h>

#define TIMx_IRQHandler     JOIN3(TIM, TIMn, _IRQHandler)

void TIMx_IRQHandler(void);

#endif

