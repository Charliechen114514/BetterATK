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

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/* init the wwdog callback */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{   
    __HAL_RCC_WWDG_CLK_ENABLE();    
    HAL_NVIC_SetPriority(WWDG_IRQn,2,3);    
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
}



