#include "pwm.h"
#include <memory.h>

void blank_pwm_init_package(PWM_InitPackage* package){
    memset(package, 0, sizeof(PWM_InitPackage));
}

void blank_pwm_timer_initer(TIM_Base_InitTypeDef* types)
{
    memset(types, 0, sizeof(TIM_Base_InitTypeDef));
}

extern CCPWM* current_hooking_pwm;
extern void(*pwm_post_init)(void);
void init_pwm(
    CCPWM* pwm, TIM_TypeDef* who, TIM_Base_InitTypeDef* base_info, 
    PWM_InitPackage* package, void(*__post_init)(void))
{
    memset(&pwm->handles, 0, sizeof(TIM_HandleTypeDef));
    pwm->handles.Instance = who;
    pwm->handles.Init = *base_info;
    current_hooking_pwm = pwm;
    pwm_post_init = __post_init;
    pwm->pwm_channel = package->channel;
    HAL_TIM_PWM_Init(&pwm->handles);
    HAL_TIM_PWM_ConfigChannel(&pwm->handles, &package->handle, package->channel);
}

void start_pwm(CCPWM* pwm)
{
    HAL_TIM_PWM_Start(&pwm->handles, pwm->pwm_channel);
}

void stop_pwm(CCPWM* pwm)
{
    HAL_TIM_PWM_Stop(&pwm->handles, pwm->pwm_channel);
}

void set_compare_level(CCPWM* pwm, uint32_t level)
{
    TIM_TypeDef*    types = pwm->handles.Instance;
    switch(pwm->pwm_channel){
        case TIM_CHANNEL_1:
            types->CCR1 = level;
        break;
        case TIM_CHANNEL_2:
            types->CCR2 = level;
        break;
        case TIM_CHANNEL_3:
            types->CCR3 = level;
        break;
        case TIM_CHANNEL_4:
            types->CCR4 = level;
        break;        
    }
}
