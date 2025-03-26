#include "pwm.h"

CCPWM* current_hooking_pwm;
void(*pwm_post_init)(void);

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if(&current_hooking_pwm->handles == htim){
        pwm_post_init();
    }
}


