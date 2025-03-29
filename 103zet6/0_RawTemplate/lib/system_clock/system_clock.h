#ifndef SYSCLK_H
#define SYSCLK_H
#include <stm32f1xx_hal.h>

/*
    This is the clk tree params interface
    if you are lazy in configured, use defaults
*/
void sys_stm32_clock_init(uint32_t plln);

#endif