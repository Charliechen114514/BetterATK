#include "timer.h"
#include <memory.h>
static uint32_t __fetch_apb_seconds(TIM_TypeDef* TIMx)
{
    uint32_t pclk_freq;

    if (TIMx == TIM1 || TIMx == TIM8 || TIMx == TIM9 || 
        TIMx == TIM10 || TIMx == TIM11) {
        pclk_freq = HAL_RCC_GetPCLK2Freq();
    } else {
        pclk_freq = HAL_RCC_GetPCLK1Freq();
    }

    if ((TIMx->PSC & 0x01) == 0) {
        return pclk_freq;  
    } else {
        return pclk_freq * 2;
    }
}

uint32_t    get_timer_mseconds(CCTimer* timer){
    uint64_t freq = __fetch_apb_seconds(timer->tim_handler.Instance);
    uint64_t pres = timer->tim_handler.Init.Prescaler + 1;
    uint64_t preroid = timer->tim_handler.Init.Period + 1;
    uint64_t clk = (pres) * (preroid) * 1000;
    return clk / freq;
}

void timer_blank_initer(TIM_Base_InitTypeDef* base_info)
{
    memset(base_info, 0, sizeof(TIM_Base_InitTypeDef));
}

extern CCTimer* current_hooking_timer;
extern void(*timer_post_init)(void);
extern CCTimer*        boarding_timers[BOARDING_TIMER_N];
extern uint8_t         current_boarding;

void init_cctimer(CCTimer* timer, 
    TIM_TypeDef* who, TIM_Base_InitTypeDef* base_info, 
    void(*enable)(void))
{
    memset(timer, 0, sizeof(CCTimer));
    timer->tim_handler.Instance = who;
    timer->tim_handler.Init = *base_info;
    timer_post_init = enable;
    current_hooking_timer = timer;
    boarding_timers[current_boarding] = timer;
    current_boarding++;
    HAL_TIM_Base_Init(&timer->tim_handler);
}

void set_cctimer_hook(CCTimer* timer, void(*callbacks)(CCTimer*))
{
    timer->callbacks = callbacks;
}

void start_timer(CCTimer* timer)
{
    HAL_TIM_Base_Start_IT(&timer->tim_handler);
}

void stop_timer(CCTimer* timer)
{
    HAL_TIM_Base_Stop_IT(&timer->tim_handler);
}

