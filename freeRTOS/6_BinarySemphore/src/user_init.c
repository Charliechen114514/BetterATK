#include "user_init.h"
#include "system_clock.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
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

static void __enable_uartgpio(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

static void __enable_uart(){
    __HAL_RCC_USART1_CLK_ENABLE();
}

UART_AFGPIO_Info info = {
    .alternatives = GPIO_AF7_USART1,
    .tx = {
        .pinType = GPIO_PIN_9,
        .port = GPIOA
    },
    .rx = {
        .pinType = GPIO_PIN_10,
        .port = GPIOA
    },
    .uart_gpioclk_enabled = __enable_uartgpio,
    .uart_self_enabled = __enable_uart
};


CCGPIOType led0, led1;
void init_bsp_led(void)
{
    configure_ccgpio(&led0, &led0_init);
    configure_ccgpio(&led1, &led1_init);
}

CCUART  uart1;
void init_uart(void)
{
    configure_common_uart(&uart1, USART1, 115200, &info, UART_SYNC, HAL_MAX_DELAY);
}

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

static KeyPressStatus detect_method(KeyPort* port){
    return fetch_sync_gpiostate(&(port->gpio_core)) == CCGPIO_HIGH;
}
KeyPort key1, key2;

void init_bsp_key(void)
{
    init_key(&key1, NULL, detect_method, &key1_init);
    init_key(&key2, NULL, detect_method, &key2_init);
}


