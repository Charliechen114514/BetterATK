/*
    Main Entry for the key detections
*/
#include "gpio.h"
#include "key.h"
#include "system_clock.h"
static void __open_gpioclk(){
    __HAL_RCC_GPIOF_CLK_ENABLE();
}

static void __open_keyclk(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
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

CCGPIOInitTypeDef key1_init = {
    .type = {
        .Pin = GPIO_PIN_0,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH,
        .Pull = GPIO_PULLDOWN
    },
    .open_clock = __open_keyclk,
    .post_init = NULL,
    .port = GPIOA
};

CCGPIOInitTypeDef key2_init = {
    .type = {
        .Pin = GPIO_PIN_4,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH,
        .Pull = GPIO_PULLDOWN
    },
    .open_clock = __open_keyclk,
    .post_init = NULL,
    .port = GPIOE
};

CCGPIOType led0, led1;
static void do_reserve_led0(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led0);
    system_delay_ms(100);
}

static void do_reserve_led1(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led1);
    system_delay_ms(100);
}

static KeyPressStatus detect_method(KeyPort* port){
    return fetch_sync_gpiostate(&(port->gpio_core)) == CCGPIO_HIGH;
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    
    configure_ccgpio(&led0, &led0_init);
    configure_ccgpio(&led1, &led1_init);
    set_ccgpio_state(&led1, CCGPIO_LOW);
    KeyPort key1, key2;
    init_key(&key1, do_reserve_led0, detect_method, &key1_init);
    init_key(&key2, do_reserve_led1, detect_method, &key2_init);
    while(1){
        doKeyCallbackIfDone(&key1, KEY_LONG_PRESS);
        doKeyCallbackIfDone(&key2, KEY_LONG_PRESS);
    }
    return 0;
}
