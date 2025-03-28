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
WindowWatchDogHandler handler;

int cnt = 0;

void my_callback(WindowWatchDogHandler* handler){
    cnt++;
    if(cnt > 10){
        cnt = 0;
        reverse_ccgpio_pinstate(&led1);
    }
    HAL_WWDG_Refresh(&handler->handle);
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    init_window_watchdog_handler(
        &handler, WWDG, WWDG_PRESCALER_8, 0X7F, 0X5F, my_callback
    );
    set_ccgpio_state(&led0, CCGPIO_LOW);
    system_delay_ms(500);
    
    
    while(1){
        set_ccgpio_state(&led0, CCGPIO_HIGH);
    }

    return 0;
}
