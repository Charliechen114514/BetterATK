/*
    These are the HAL Handler callbacks, so add the interface callbacks here
    if you requires the self-defined it callbacks
*/

// for special reasons, Systicks_Handler is regsitered in system_clock.c
#include "stm32f4xx_hal.h"


__attribute__((weak)) void SysTick_Handler(void){
    // Add default definitions here
    // like 
    HAL_IncTick(); 
}

