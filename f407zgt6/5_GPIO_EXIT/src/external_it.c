#include "external_it.h"
extern EXTI_Handler*       memorizingTypes[MAX_MEMORIZE_TYPE_N];
extern uint8_t             actual_memorize;

void init_exti_handler(
    EXTI_Handler* handler, 
    CCGPIOInitTypeDef* initer,
    EXTIConfigPack* pack,
    void(*callback)(EXTI_Handler*))
{
    configure_ccgpio(&(handler->gpio_info), initer);
    handler->callback = callback;
    HAL_NVIC_SetPriority(pack->irq, pack->preempt_level, pack->sub_level);
    HAL_NVIC_EnableIRQ(pack->irq);
    memorizingTypes[actual_memorize] = handler;
    actual_memorize++;
}