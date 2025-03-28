/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
#include "timer.h"
#include "pwm.h"
extern CCGPIOType   led0, led1;
extern CCUART       uart1;
extern CCTimer      timer;
extern CCPWM        pwm;
void timer_callback(CCTimer* timer){
    (void)timer;
    reverse_ccgpio_pinstate(&led1);
}
#include <stdio.h>
int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    pwm_init();
    start_pwm(&pwm);
    int level_value = 0;
    int level_creasing = 1;
    while(1){
        system_delay_ms(2);
        level_value += level_creasing;
        if(level_value > 300) 
            level_creasing = -1;
        else if(level_value == 0)
            level_creasing = 1;
        set_compare_level(&pwm, level_value);
    }

    return 0;
}
