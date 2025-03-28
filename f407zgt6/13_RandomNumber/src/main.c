/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include <stdio.h>
#include "rand.h"
extern CCGPIOType led0, led1;
extern CCUART uart1;
CCGraphicTextEdit text;

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    rand_init();
    CCDevice_t* device = get_softiic_inited_handle();
    set_simple_text(&text, device, "hello!");
    on_set_hander_of_line(&text, 1);
    set_simple_text(&text, device, "hello! newline!");
    char meg[20];
    int randn = 0;
    while (1)
    {
        randn = fetch_rand_inrange(0, 100);
        snprintf(meg, 20, "Rand num fetched: %d", randn);
        on_set_hander_of_line(&text, 2);
        set_simple_text(&text, device, meg);
        text.operations.clear(&text);
        system_delay_ms(100);        
    }
    return 0;
}
