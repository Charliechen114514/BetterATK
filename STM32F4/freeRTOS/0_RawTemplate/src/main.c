/*
    Main Entry for the LED Lighten Up
*/
#include "gpio.h"
#include "system_clock.h"
#include "include/FreeRTOS.h"
#include "include/task.h"
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

CCGPIOType led0, led1;

void gpio_reverse_task(void *params)
{
    CCGPIOType* gpio = (CCGPIOType*)(params);
    for (;;)
    {
        reverse_ccgpio_pinstate(gpio);
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

int main(void)
{
    // we need to set the HAL NVIC Settings for the group4
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK, TIM6);
    
    configure_ccgpio(&led0, &led0_init);
    configure_ccgpio(&led1, &led1_init);
    set_ccgpio_state(&led1, CCGPIO_LOW);
    xTaskCreate(gpio_reverse_task, "LED0_Task", 128, &led0, 1, NULL);
    xTaskCreate(gpio_reverse_task, "LED1_Task", 128, &led1, 1, NULL);
    vTaskStartScheduler();
    while (1)
    {
        // reverse_ccgpio_pinstate(&led0);
        // reverse_ccgpio_pinstate(&led1);
        // system_delay_ms(500);
    }
    
    return 0;
}
