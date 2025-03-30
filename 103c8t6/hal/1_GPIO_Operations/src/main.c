#include "system_clock.h"
#include "gpio.h"

static void gpio_clk_open()
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

static void led_post_init(CCGPIOType* init_type)
{
    set_ccgpio_pinstate(init_type, CCGPIO_HIGH);
}

CCGPIO_InitType led0_initer = {
    .clk_open = gpio_clk_open,
    .post_init = led_post_init,
    .port = GPIOC,
    .initer = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = GPIO_PIN_13,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Pull = GPIO_PULLUP
    }
};


int main()
{
    HAL_Init();
    system_clock_init();
    CCGPIOType      led0;
    configure_ccgpio(&led0, &led0_initer);

    while(1)
    {
        reverse_ccgpio_pinstate(&led0);
        system_delay_ms(200);
    }
}