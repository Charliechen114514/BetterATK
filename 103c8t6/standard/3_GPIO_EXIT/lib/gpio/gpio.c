/**
 * @file gpio.c
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2025-03-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "gpio.h"


/**
 * @brief this configures the GPIO, make settings in the CCGPIO_InitType
 * 
 * @param[out]  type    containers for the gpio, you can pass NULL for unused handle
 * @param[in]   initer  must be passed for telling how to init the issue
 */
void configure_ccgpio(CCGPIOType* type, CCGPIO_InitType* initer)
{
    CCGPIOType cached;
    cached.pin = initer->initer.GPIO_Pin;
    cached.port = initer->port;
    initer->clk_open(type);
    GPIO_Init(initer->port, &initer->initer);
    if(type){
        *type = cached;
    }
    if(initer->post_init){
        initer->post_init(&cached);
    }
}


/**
 * @brief Set the ccgpio pinstate object for a concrete gpio level
 * 
 * @param[in] type  gpio_handle
 * @param[in] state states of the gpio pinstate
 */
void set_ccgpio_pinstate(CCGPIOType* type, CCGPIO_PinState state){
    GPIO_WriteBit(type->port, type->pin, (BitAction)(state));
}

/**
 * @brief reverse the gpio pin state, or we say: toggle the pin!
 * 
 * @param type gpio_handle
 */
void reverse_ccgpio_pinstate(CCGPIOType* type)
{
    /* get current Ouput Data Register value */
    uint32_t odr = type->port->ODR;
    /* Set selected pins that were at low level, and reset ones that were high */
    type->port->BSRR = (
        (odr & type->pin) << (16u)) | (~odr & type->pin);
}

CCGPIO_PinState fetch_pin_status(CCGPIOType* type){
    return (CCGPIO_PinState)GPIO_ReadInputDataBit(type->port, type->pin);
}