/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
#include "external_it.h"

extern CCGPIOType led0, led1;
extern CCUART  uart1;
extern KeyPort key1, key2;

void do_reserve_led0(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led0);
    system_delay_ms(100);
}

void do_reserve_led1(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led1);
    system_delay_ms(100);
}

static void __open_exit1_clk(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

static void __open_exit2_clk(){
    __HAL_RCC_GPIOE_CLK_ENABLE();
}

CCGPIOInitTypeDef exti1_init = {
    .type = {
        .Pin = GPIO_PIN_0,
        .Mode = GPIO_MODE_IT_RISING,
        .Speed = GPIO_SPEED_HIGH,
        .Pull = GPIO_PULLDOWN
    },
    .open_clock = __open_exit1_clk,
    .post_init = NULL,
    .port = GPIOA
};

CCGPIOInitTypeDef exti2_init = {
    .type = {
        .Pin = GPIO_PIN_4,
        .Mode = GPIO_MODE_IT_RISING,
        .Speed = GPIO_SPEED_HIGH,
        .Pull = GPIO_PULLUP
    },
    .open_clock = __open_exit2_clk,
    .post_init = NULL,
    .port = GPIOE
};

EXTIConfigPack key1_intr = {
    .irq = EXTI0_IRQn,
    .preempt_level = 2,
    .sub_level = 0
};

EXTIConfigPack key2_intr = {
    .irq = EXTI4_IRQn,
    .preempt_level = 2,
    .sub_level = 3
};

void exti1_callback(EXTI_Handler* unused){
    (void)unused;
    key1.key_callback(&key1);
}

void exti2_callback(EXTI_Handler* unused){
    (void)unused;
    key2.key_callback(&key2);
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);

    led_setup();
    setup_usart();
    key_init();

    EXTI_Handler    exti1, exti2;
    init_exti_handler(&exti1, &exti1_init, &key1_intr, exti1_callback);
    init_exti_handler(&exti2, &exti2_init, &key2_intr, exti2_callback);

    send_message(&uart1, "Test on the EXIT Experiment\n");
    set_ccgpio_state(&led0, CCGPIO_LOW);
    
    while(1){
    }

    return 0;
}
