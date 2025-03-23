#include "system_init.h"
#include "system_clock.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
#include "stm32f4xx_hal.h"

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

CCUART  uart1;
void setup_usart()
{
    configure_common_uart(&uart1, USART1, 115200, &info, UART_SYNC, HAL_MAX_DELAY);
}


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

// LED Sources
CCGPIOType led0, led1;

void led_setup()
{
    configure_ccgpio(&led0, &led0_init);
    configure_ccgpio(&led1, &led1_init);
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

void do_reserve_led1(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led1);
    system_delay_ms(100);
}

void do_reserve_led0(KeyPort* port)
{
    reverse_ccgpio_pinstate(&led0);
    system_delay_ms(100);
}

KeyPort key1, key2;

void key_init()
{
    init_key(&key1, do_reserve_led0, detect_method, &key1_init);
    init_key(&key2, do_reserve_led1, detect_method, &key2_init);
}

#include "timer.h"
CCTimer     timer;

void my_timer_msp_hook(void){
    __HAL_RCC_TIM3_CLK_ENABLE();            
    HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    
    HAL_NVIC_EnableIRQ(TIM3_IRQn);          
}

void timer_init()
{
    TIM_Base_InitTypeDef    initer;
    timer_blank_initer(&initer);
    initer.Period = 5000 - 1;
    initer.Prescaler = 8400 - 1;
    initer.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    initer.CounterMode = TIM_COUNTERMODE_UP;
    init_cctimer(&timer, TIM3, &initer, my_timer_msp_hook);
}

#include "pwm.h"

static void __open_pwm_clk(){
	__HAL_RCC_TIM13_CLK_ENABLE();			
	__HAL_RCC_GPIOF_CLK_ENABLE();			
}

CCGPIOInitTypeDef pwm_gpio_init = {
    .post_init = NULL,
    .open_clock = __open_pwm_clk,
    .port = GPIOF,
    .type = {
        .Alternate = GPIO_AF9_TIM13,
        .Mode = GPIO_MODE_AF_PP,
        .Pin = GPIO_PIN_8,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_HIGH
    }
};
CCGPIOType pwm_gpio;
void my_post_init()
{
    configure_ccgpio(&pwm_gpio, &pwm_gpio_init);
}


CCPWM       pwm;
void pwm_init()
{
    TIM_Base_InitTypeDef    initer;
    PWM_InitPackage pack;
    blank_pwm_timer_initer(&initer);
    initer.Prescaler= 84 - 1;      
    initer.CounterMode=TIM_COUNTERMODE_UP;
    initer.Period= 500 - 1;          
    initer.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    blank_pwm_init_package(&pack);
    pack.channel = TIM_CHANNEL_1;
    pack.handle.OCMode=TIM_OCMODE_PWM1;
    pack.handle.Pulse= 250;
    pack.handle.OCPolarity=TIM_OCPOLARITY_LOW;
    init_pwm(&pwm, TIM13, &initer, &pack, my_post_init);
}