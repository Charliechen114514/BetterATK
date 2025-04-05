#ifndef GPIO_EXTI_H
#define GPIO_EXTI_H

#include "precheck.h"
#include "gpio.h"
#define MAX_CACHED      (3)
typedef enum {
    GPIO_EXTI_INTR,
    GPIO_EXTI_EVENT
}TrigType;

typedef enum {
    GPIO_EXTI_RISING,
    GPIO_EXTI_FALLING,
    GPIO_EXTI_BOTH_EDGE
}TrigMode;


typedef struct {
    struct {
        uint8_t PortSources;
        uint8_t pinSources;
    }configure_sources;
    TrigType        type;
    TrigMode        mode;
    uint32_t        exit_line;
    struct {
        uint8_t     line_irq_num;
        uint8_t     main_preepmt;
        uint8_t     sub_preempt;
    }IRQSettings;
}CCGPIO_ExtiLineInit;

/**
 * @brief use this functions to submit a nvic/gpio intrs
 * 
 * @param[in] line: filled the struct and submit to the systems!
 */
void configure_gpio_exitline(CCGPIO_ExtiLineInit* line);

/* Hooks should be kept for the own */
typedef void(*GPIOEXTI_HookType)(CCGPIOType* types);

void register_exitmode(GPIOEXTI_HookType hook, CCGPIOType* who);

// raw hook prechecking sources
typedef struct {
    uint32_t    exti_line;
}HookPrecheck;

/**
 * @brief 
 * 
 * @param precheck  precheck should submit the raw lines-irq, the lib will
 *                  help clear the pending flags
 * @param types     the gpio for calling backs
 */
void do_gpio_exti_hook(HookPrecheck* precheck, CCGPIOType* types);

void configure_nvic_group(uint32_t groups);

#endif