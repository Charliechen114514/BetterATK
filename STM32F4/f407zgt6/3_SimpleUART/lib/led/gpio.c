#include "gpio.h"

static void __die(){
    while(1);
}

void configure_ccgpio(
    CCGPIOType* type,  CCGPIOInitTypeDef* initer)
{
    type->port = initer->port;
    type->pinType = initer->type.Pin;

    // oh shit, the open clock is missing inited
    initer->open_clock ? initer->open_clock() : __die();

    HAL_GPIO_Init(type->port, &initer->type);

    if(initer->post_init) initer->post_init(type);
}

void set_ccgpio_state(CCGPIOType* type, CCGPIOState state)
{
    HAL_GPIO_WritePin(
        type->port, type->pinType, 
        (state ? (GPIO_PIN_SET) : (GPIO_PIN_RESET)));
}

void reverse_ccgpio_pinstate(CCGPIOType* type)
{
    HAL_GPIO_TogglePin(type->port, type->pinType);
}

CCGPIOState fetch_sync_gpiostate(CCGPIOType* type)
{
    return (CCGPIOState)HAL_GPIO_ReadPin(type->port, type->pinType);
}