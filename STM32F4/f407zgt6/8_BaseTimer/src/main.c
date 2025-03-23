/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
#include "timer.h"
extern CCGPIOType   led0, led1;
extern CCUART       uart1;
extern CCTimer      timer;

void timer_callback(CCTimer* timer){
    (void)timer;
    reverse_ccgpio_pinstate(&led1);
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    timer_init();
    set_cctimer_hook(&timer, timer_callback);
    start_timer(&timer);
    
    while(1){
        reverse_ccgpio_pinstate(&led0);
        system_delay_ms(300);
    }

    return 0;
}
