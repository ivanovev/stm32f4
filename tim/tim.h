
#ifndef __TIM_H__
#define __TIM_H__

#define TIMx_CLK_ENABLE     JOIN3(__TIM, TIMn, _CLK_ENABLE)
#define TIMx_IRQn           JOIN3(TIM, TIMn, _IRQn)
#define TIMx_FORCE_RESET JOIN3(__TIM, TIMn, _FORCE_RESET)
#define TIMx_RELEASE_RESET JOIN3(__TIM, TIMn, _RELEASE_RESET)

void tim_init(void);
void tim_deinit(void);
uint32_t tim_get_prescaler(TIM_TypeDef *tim);

#endif

