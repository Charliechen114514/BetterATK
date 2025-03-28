#include "stm32f4xx_hal.h"
#include "timer.h"


CCTimer* current_hooking_timer;
void(*post_init)(void);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim==&current_hooking_timer->tim_handler){
        post_init();
    }
}

CCTimer*        boarding_timers[BOARDING_TIMER_N];
uint8_t         current_boarding;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for(uint8_t i = 0; i < current_boarding; i++){
        if(&boarding_timers[i]->tim_handler == htim){
            if(boarding_timers[i]->callbacks)
                boarding_timers[i]->callbacks(boarding_timers[i]);
        }
    }
}

void TIM3_IRQHandler(void)
{
    for(uint8_t i = 0; i < current_boarding; i++){
        if(boarding_timers[i]->tim_handler.Instance == TIM3){
            HAL_TIM_IRQHandler(&boarding_timers[i]->tim_handler);
        }
    }
}