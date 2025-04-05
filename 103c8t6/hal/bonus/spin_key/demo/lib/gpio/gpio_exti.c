#include "gpio_exti.h"

struct
{
    GPIOEXTI_HookType callbacks;
    CCGPIOType matchings;
} hooks_cached[GPIO_EXTI_HOOK_MAX_CACHED];
short current_gpioexti_hook_size;

static void __register_hooks(GPIOEXTI_HookType callbacks, CCGPIOType *types)
{
    hooks_cached[current_gpioexti_hook_size].callbacks = callbacks;
    hooks_cached[current_gpioexti_hook_size].matchings = *types;
    current_gpioexti_hook_size++;
}

void configure_gpio_exitline(CCGPIO_ExtiLineInit *line)
{
    __register_hooks(line->callbackHook, &line->which_gpio);
    HAL_NVIC_SetPriority(line->interrupt_settings.irq,
                         line->interrupt_settings.preempt_level,
                         line->interrupt_settings.sub_level);
    HAL_NVIC_EnableIRQ(line->interrupt_settings.irq);
}


void do_gpio_exti_hook(CCGPIOType* types)
{
    if(__HAL_GPIO_EXTI_GET_IT(types->pin) == 0x00){
        return;
    }
    __HAL_GPIO_EXTI_CLEAR_IT(types->pin);
    for(short i = 0; i < current_gpioexti_hook_size; i++)
    {
        if(
            types->pin == hooks_cached[i].matchings.pin &&
            types->port == hooks_cached[i].matchings.port){
                hooks_cached[i].callbacks(types);
                return;
        }
    }
}
