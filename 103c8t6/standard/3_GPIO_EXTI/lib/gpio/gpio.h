/**
 * @file gpio.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef GPIO_H
#define GPIO_H
#include "precheck.h"

typedef uint32_t GPIO_PinType;
typedef struct __gpiofullinit CCGPIO_InitType;
typedef struct __gpiomintype CCGPIOType;

typedef void(*OpenClock)(CCGPIOType*);
typedef void(*GPIOPostInit)(CCGPIOType*);

/**
 * @brief this is a simple minial gpio type
 */
typedef struct __gpiomintype{
    GPIO_TypeDef*       port;   ///<    gpio port
    GPIO_PinType        pin;    ///<    gpio pin
}CCGPIOType;

/**
 * @brief this is a initer of gpio
 */
typedef struct __gpiofullinit{
    OpenClock               clk_open;   ///< we need to provide a open clock as neccessities
    GPIOPostInit            post_init;  ///< as optional choices as some init don't require the post init
    GPIO_TypeDef*           port;       ///< Memorize the port we open clk
    GPIO_InitTypeDef        initer;     ///< initer of the gpio type
}CCGPIO_InitType;

/**
 * @brief this configures the GPIO, make settings in the CCGPIO_InitType
 * 
 * @param[out]  type    containers for the gpio, you can pass NULL for unused handle
 * @param[in]   initer  must be passed for telling how to init the issue
 */
void configure_ccgpio(CCGPIOType* type, CCGPIO_InitType* initer);

/**
 * @brief   this is a simple enum of the PinState, 
 *          As expected:
 *          CCGPIO_LOW offer a low level output
 *          CCGPIO_HIGH offer a high level output
 */
typedef enum{
    CCGPIO_LOW,
    CCGPIO_HIGH
}CCGPIO_PinState;

/**
 * @brief Set the ccgpio pinstate object for a concrete gpio level
 * 
 * @param[in] type  gpio_handle
 * @param[in] state states of the gpio pinstate
 */
void set_ccgpio_pinstate(CCGPIOType* type, CCGPIO_PinState state);

/**
 * @brief reverse the gpio pin state, or we say: toggle the pin!
 * 
 * @param type gpio_handle
 */
void reverse_ccgpio_pinstate(CCGPIOType* type);
/**
 * @brief read the gpio pin status
 * 
 * @param type      the gpio target
 * @return CCGPIO_PinState  result of reading
 */
CCGPIO_PinState fetch_pin_status(CCGPIOType* type);

#endif