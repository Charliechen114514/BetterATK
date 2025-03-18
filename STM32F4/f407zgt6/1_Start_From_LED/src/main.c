/*
    Main Entry for the LED Lighten Up
*/
#include "gpio.h"
#include "sysclock.h"
#include "delay.h"
static void __open_gpioclk(){
    __HAL_RCC_GPIOF_CLK_ENABLE();
}

static void __post_init(CCGPIOType* type){set_ccgpio_state(type, CCGPIO_HIGH);}

CCGPIOInitTypeDef led0_init = {
    .type = {
        .Pin    = GPIO_PIN_9,
        .Mode   = GPIO_MODE_OUTPUT_PP,
        .Speed  = GPIO_SPEED_HIGH,
        .Pull   = GPIO_PULLUP
    },
    .open_clock = __open_gpioclk,
    .post_init  = __post_init,
    .port = GPIOF
};

CCGPIOInitTypeDef led1_init = {
    .type = {
        .Pin    = GPIO_PIN_10,
        .Mode   = GPIO_MODE_OUTPUT_PP,
        .Speed  = GPIO_SPEED_HIGH,
        .Pull   = GPIO_PULLUP
    },
    .open_clock = __open_gpioclk,
    .post_init  = __post_init,
    .port = GPIOF
};

int main(void)
{
    HAL_Init();
    sysclk_set_as_common168MHz();
    init_delay();
    CCGPIOType led0, led1;
    configure_ccgpio(&led0, &led0_init);
    configure_ccgpio(&led1, &led1_init);
    set_ccgpio_state(&led1, CCGPIO_LOW);
    while(1){
        reverse_ccgpio_pinstate(&led0);
        reverse_ccgpio_pinstate(&led1);
        delay_ms(500);
    }
    return 0;
}
