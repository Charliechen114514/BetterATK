/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
#include "external_it.h"
#include "watchdog.h"
extern CCGPIOType led0, led1;
extern CCUART  uart1;
extern KeyPort key2;

KeyPort key1;
WatchDogHandler watchdog_handle;

static void __open_keyclk(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
}

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

static KeyPressStatus detect_method(KeyPort* port){
    return fetch_sync_gpiostate(&(port->gpio_core)) == CCGPIO_HIGH;
}

void do_reserve_led1(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led1);
    system_delay_ms(100);
}

void do_feeddog(KeyPort* port){
    feed_watchdog(&watchdog_handle);
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);

    led_setup();
    setup_usart();
    key_init();
    init_key(&key1, do_feeddog, detect_method, &key1_init);
    system_delay_ms(500);
    init_watchdog_handler(&watchdog_handle, IWDG, IWDG_PRESCALER_64, 500);
    set_ccgpio_state(&led0, CCGPIO_LOW);
    
    while(1){
        doKeyCallbackIfDone(&key1, KEY_LONG_PRESS);
        doKeyCallbackIfDone(&key2, KEY_LONG_PRESS);
    }

    return 0;
}
