#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx_hal.h"
#define     BOARDING_TIMER_N            (1)
typedef struct __cctimer CCTimer;
typedef struct __cctimer{
    TIM_HandleTypeDef       tim_handler;
    void(*callbacks)(CCTimer*);
}CCTimer;

void        timer_blank_initer(TIM_Base_InitTypeDef* base_info);
uint32_t    get_timer_mseconds(CCTimer* timer);

void init_cctimer(
    CCTimer* timer, TIM_TypeDef* who, TIM_Base_InitTypeDef* base_info, void(*enable)(void));
void set_cctimer_hook(CCTimer* timer, void(*callbacks)(CCTimer*));
void start_timer(CCTimer* timer);



#endif