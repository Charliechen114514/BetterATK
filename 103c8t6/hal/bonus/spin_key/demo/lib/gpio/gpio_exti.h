#ifndef GPIO_EXTI_H
#define GPIO_EXTI_H

#include "gpio.h"
#define GPIO_EXTI_HOOK_MAX_CACHED       (3)
/* Hooks should be kept for the own */
typedef void(*GPIOEXTI_HookType)(CCGPIOType* types);

typedef struct {
    GPIOEXTI_HookType   callbackHook;
    CCGPIOType          which_gpio;
    struct {
        IRQn_Type       irq;
        uint8_t         preempt_level;
        uint8_t         sub_level;
    }interrupt_settings;
}CCGPIO_ExtiLineInit;


/**
 * @brief use this functions to submit a nvic/gpio intrs
 * 
 * @param[in] line: filled the struct and submit to the systems!
 */
void configure_gpio_exitline(CCGPIO_ExtiLineInit* line);

/**
 * @brief   
 *  do hook provides a gpio exti, 
 *  use this hook in HAL Callbacks as direct
 * @param types 
 */
void do_gpio_exti_hook(CCGPIOType* types);

#endif