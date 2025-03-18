/*
    Main Entry for the key detections
*/
#include "ccuart.h"
#include "system_clock.h"
#include "gpio.h"
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

static void __open_gpioclk(){
    __HAL_RCC_GPIOF_CLK_ENABLE();
}
static void __post_init(CCGPIOType* type){set_ccgpio_state(type, CCGPIO_HIGH);}


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
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);

    CCUART  uart1;
    CCGPIOType led1;
    UARTSendBuffer buffer;
    configure_ccgpio(&led1, &led1_init);
    makeup_uartsend_buffer(&buffer, "hello\n", UART_EXTSRC_ALL);
    configure_common_uart(&uart1, USART1, 115200, &info, UART_SYNC, HAL_MAX_DELAY);
    while(1){
        send_src_uart_sync(&uart1, &buffer);
        receive_src_uart_sync(&uart1, &buffer);
        reverse_ccgpio_pinstate(&led1);
    }

    return 0;
}
