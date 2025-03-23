#ifndef PWM_H
#define PWM_H
#include "stm32f4xx_hal.h"
typedef struct __pwm_struct {
    TIM_HandleTypeDef       handles;
    int                     pwm_channel;
}CCPWM;

typedef struct {
    TIM_OC_InitTypeDef handle;
    uint32_t channel;
}PWM_InitPackage;

void blank_pwm_init_package(PWM_InitPackage* package);
void blank_pwm_timer_initer(TIM_Base_InitTypeDef* types);

void init_pwm(
    CCPWM* pwm, TIM_TypeDef* who, TIM_Base_InitTypeDef* base_info, 
    PWM_InitPackage* package, void(*post_init)(void));

void start_pwm(CCPWM* pwm);
void stop_pwm(CCPWM* pwm);

void set_compare_level(CCPWM* pwm, uint32_t level);

#endif

