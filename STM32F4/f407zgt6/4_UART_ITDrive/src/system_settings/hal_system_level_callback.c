/*
    These are the HAL Handler callbacks, so add the interface callbacks here
    if you requires the self-defined it callbacks
*/

// for special reasons, Systicks_Handler is regsitered in system_clock.c
#include "stm32f4xx_hal.h"
#include "ccuart.h"

// For any HAL Application Level systicks
// reset this for user defined level
// Don't do anything related to the systicks
// else it maybe crashed and dumped into WatchDog error
void SysTick_Handler(void)
{
    HAL_IncTick();
}
