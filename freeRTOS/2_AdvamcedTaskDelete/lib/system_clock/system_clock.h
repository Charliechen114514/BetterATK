#ifndef SYSCLK_H
#define SYSCLK_H
#include <stm32f4xx_hal.h>
#include "system_clock_preference.h"
/*
    This is the clk tree params interface
    if you are lazy in configured, use defaults
*/
typedef struct __Sysclk_ppl_config{
    uint32_t    n;
    uint32_t    m;
    uint32_t    p;
    uint32_t    q;
}CCCLK_SysConfigType;

void        system_clock_init(
        const CCCLK_SysConfigType* sysconfig, 
        const ClockSourcePreference source,         
#ifndef USDELAY_USE_SYSCLK
        TIM_TypeDef* TIM_HANDLE
#endif
);

#define USE_SYSCLK  (NULL)
void        system_clock_initAs168MHz(
    const ClockSourcePreference source, 
#ifndef USDELAY_USE_SYSCLK
    TIM_TypeDef* TIM_HANDLE
#endif
);
/* clock services */
uint32_t    provide_application_sysfreq();
void        system_delay_ms(uint32_t delay_ms);
void        system_delay_us(uint32_t delay_us);

#endif