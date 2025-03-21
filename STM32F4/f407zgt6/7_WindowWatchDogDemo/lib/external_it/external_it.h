#ifndef EXTERTNAL_IT
#define EXTERTNAL_IT
#include "gpio.h"

#define MAX_MEMORIZE_TYPE_N         (5)

typedef struct __exit_handler{
    CCGPIOType      gpio_info;
    void(*callback)(struct __exit_handler*);
}EXTI_Handler;

typedef struct {
    IRQn_Type       irq;
    uint8_t         preempt_level;
    uint8_t         sub_level;    
}EXTIConfigPack;

void init_exti_handler(
    EXTI_Handler* handler, 
    CCGPIOInitTypeDef* initer,
    EXTIConfigPack* pack,
    void(*callback)(EXTI_Handler*)  // callbacks if the interrupt triggers
);

#endif