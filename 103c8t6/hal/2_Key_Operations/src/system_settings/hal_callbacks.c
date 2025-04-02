#include "stm32f1xx_hal.h"

/**
 * @brief Systick Handler for the global
 * 
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}