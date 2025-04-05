#include "stm32f1xx_hal.h"

/**
 * @brief Systick Handler for the global
 * 
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}


/*  -------------- GPIO EXTI ---------------   */
#include "gpio_exti.h"

/* Hooks are generate in here */

#define GEN_HOOKS_CALLS(_port, _pin) \
    CCGPIOType __types = {\
        .port = _port,\
        .pin = _pin\
    }; \
    do_gpio_exti_hook(&__types);


void EXTI0_IRQHandler(void){
    GEN_HOOKS_CALLS(GPIOB, GPIO_PIN_0);
}

void EXTI1_IRQHandler(void){
    GEN_HOOKS_CALLS(GPIOB, GPIO_PIN_1);
}

void EXTI15_10_IRQHandler(void){
    GEN_HOOKS_CALLS(GPIOB, GPIO_PIN_10);
}

#undef GEN_HOOKS_CALLS