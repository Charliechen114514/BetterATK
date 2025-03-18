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
    .uart_self_enabled = __enable_uart,
    .irq_n = USART1_IRQn,
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

UARTSendBuffer buffer;
CCGPIOType led1;
UARTSendBuffer trigger_receive;
UARTSendBufferView view;
int receive = 0;
char sender[10] = {0};

void my_uart_rx_callback(CCUART* uart){
    reverse_ccgpio_pinstate(&led1);
    sender[receive] = trigger_receive.sources[0];
    receive++;
    if(receive >= 5){
        receive = 0;
        send_message(uart, "Receive Message: ");
        send_srcview_uart_sync(uart, &view);
    }
    receive_src_uart(uart, &trigger_receive);
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);

    CCUART  uart1;
    
    configure_ccgpio(&led1, &led1_init);
    makeup_uartsend_buffer(&trigger_receive, "", 1);
    makeup_uartsendview(&view, sender, 5);
    configure_common_uart(&uart1, USART1, 115200, &info, UART_IT, HAL_MAX_DELAY);
    configure_uart_itcallback(&uart1, NULL,my_uart_rx_callback);
    send_message(&uart1, "Try to press something!\n");
    receive_src_uart(&uart1, &trigger_receive);
    while(1){
    }

    return 0;
}
