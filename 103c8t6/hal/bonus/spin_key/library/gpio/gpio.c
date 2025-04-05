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
 * @brief configures the GPIO, make settings in the CCGPIO_InitType
 *
 * @param[out]  type      handle type
 * @param[in]   initer    initer
 */
void configure_ccgpio(CCGPIOType *type, CCGPIO_InitType *initer)
{
    // open clock as the first step
    CCGPIOType cached;
    cached.pin = initer->initer.Pin;
    cached.port = initer->port;
    initer->clk_open(type);
    HAL_GPIO_Init(initer->port, &initer->initer);
    if (initer->post_init)
    {
        initer->post_init(&cached);
    }

    if (type)
    {
        *type = cached;
    }
}

/**
 * @brief Set the ccgpio pinstate object for a concrete gpio level
 *
 * @param[in] type  gpio_handle
 * @param[in] state states of the gpio pinstate
 */
void set_ccgpio_pinstate(CCGPIOType *type, CCGPIO_PinState state)
{
    HAL_GPIO_WritePin(type->port, type->pin, (GPIO_PinState)(state));
}

/**
 * @brief reverse the gpio pin state, or we say: toggle the pin!
 *
 * @param type gpio_handle
 */
void reverse_ccgpio_pinstate(CCGPIOType *type)
{
    HAL_GPIO_TogglePin(type->port, type->pin);
}

CCGPIO_PinState fetch_pin_status(CCGPIOType* type)
{
    return (CCGPIO_PinState)HAL_GPIO_ReadPin(type->port, type->pin);
}