#include "gpio_exti.h"

static struct
{
    GPIOEXTI_HookType hook;
    CCGPIOType types;
} cached_hooks[MAX_CACHED];
static short cached_size;

static GPIOEXTI_HookType search_hook(CCGPIOType *types)
{
    for (uint8_t i = 0; i < cached_size; i++)
    {
        if (cached_hooks[i].types.pin == types->pin &&
            cached_hooks[i].types.port == types->port)
        {
            return cached_hooks[i].hook;
        }
    }
    return 0;
}

void do_gpio_exti_hook(HookPrecheck *precheck, CCGPIOType *types)
{
    if (EXTI_GetITStatus(precheck->exti_line) == SET)
    {
        GPIOEXTI_HookType hook = search_hook(types);
        if (hook)
            hook(types);
        EXTI_ClearITPendingBit(precheck->exti_line);
    }
}

void register_exitmode(GPIOEXTI_HookType hook, CCGPIOType *who)
{
    cached_hooks[cached_size].hook = hook;
    cached_hooks[cached_size].types = *who;
    cached_size++;
}

void configure_gpio_exitline(CCGPIO_ExtiLineInit *line)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(line->configure_sources.PortSources, line->configure_sources.pinSources);
    EXTI_InitTypeDef initer;
    initer.EXTI_Line = line->exit_line;
    initer.EXTI_LineCmd = ENABLE;
    switch (line->type)
    {
    case GPIO_EXTI_EVENT:
        initer.EXTI_Mode = EXTI_Mode_Event;
        break;
    case GPIO_EXTI_INTR:
        initer.EXTI_Mode = EXTI_Mode_Interrupt;
        break;
    }

    switch (line->mode)
    {
    case GPIO_EXTI_RISING:
        initer.EXTI_Trigger = EXTI_Trigger_Rising;
        break;
    case GPIO_EXTI_FALLING:
        initer.EXTI_Trigger = EXTI_Trigger_Falling;
        break;
    case GPIO_EXTI_BOTH_EDGE:
        initer.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
        break;
    }
    EXTI_Init(&initer);
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = line->IRQSettings.line_irq_num;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = line->IRQSettings.main_preepmt;
    nvic.NVIC_IRQChannelSubPriority = line->IRQSettings.sub_preempt;
    NVIC_Init(&nvic);
}

void configure_nvic_group(uint32_t groups)
{
    NVIC_PriorityGroupConfig(groups);
}