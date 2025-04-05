#include "spin_locker.h"

static CCSpinLockerHandle* monitoring_handle;

static void raw_hook_left(CCGPIOType* types)
{
    // left pins detect
    if(fetch_pin_status(types) == 0){
        *types = monitoring_handle->handling_types[CCSpinLocker_SPIN_RIGHT_INDEX];
        if(fetch_pin_status(types) == 0){
            monitoring_handle->hooks[CCSpinLocker_SPIN_LEFT_INDEX]();
        }
    }
}

static void raw_hook_right(CCGPIOType* types)
{
    // left pins detect
    if(fetch_pin_status(types) == 0){
        *types = monitoring_handle->handling_types[CCSpinLocker_SPIN_LEFT_INDEX];
        if(fetch_pin_status(types) == 0){
            monitoring_handle->hooks[CCSpinLocker_SPIN_RIGHT_INDEX]();
        }
    }    
}

static void raw_hook_keypress(CCGPIOType* types)
{
    monitoring_handle->hooks[CCSpinLocker_SPIN_KEY_INDEX]();
}

static void(*raw_hooks[CCSpinLockerHandle_PORTS])(CCGPIOType*) = {
    raw_hook_left,
    raw_hook_right,
    raw_hook_keypress
};

void initCCSpinLockerHandle(CCSpinLockerHandle *blank_hooks, CCSpinLockerHandleInitType *initer)
{
    /* init the GPIO */
    CCGPIO_InitType gpio_init_types;
    CCGPIOType intr_helper;
    CCGPIO_ExtiLineInit exti_init;
    gpio_init_types.post_init = NULL;
    gpio_init_types.initer.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_types.initer.Pull = GPIO_PULLUP;
    gpio_init_types.initer.Mode = GPIO_MODE_IT_FALLING;
    for (uint8_t i = 0; i < CCSpinLockerHandle_PORTS; i++)
    {
        gpio_init_types.clk_open = initer->info.internal_infos[i].open_clk;
        gpio_init_types.port = initer->info.internal_infos[i].types.port;
        gpio_init_types.initer.Pin = initer->info.internal_infos[i].types.pin;
        configure_ccgpio(&intr_helper, &gpio_init_types);
        
        blank_hooks->handling_types[i] = intr_helper;
        blank_hooks->hooks[i] = initer->hooks[i];

        exti_init.callbackHook = raw_hooks[i];

        exti_init.interrupt_settings.irq =
            initer->info.internal_infos[i].irq_settings.irq_lines;
        
        exti_init.interrupt_settings.preempt_level = 
            initer->info.internal_infos[i].irq_settings.main_preepmt;
        
        exti_init.interrupt_settings.sub_level = 
            initer->info.internal_infos[i].irq_settings.sub_preempt;

        exti_init.which_gpio = intr_helper;
        configure_gpio_exitline(&exti_init);
    }
    monitoring_handle = blank_hooks;
}

void change_monitor_handle(CCSpinLockerHandle* blank_hooks)
{
    monitoring_handle = blank_hooks;
}