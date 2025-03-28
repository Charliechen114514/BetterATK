/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include <stdio.h>
#include "adc.h"
extern CCGPIOType led0, led1;
extern CCUART uart1;
extern CCADC_Handle    adc_handle;
CCGraphicTextEdit text;

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    init_adc();
    CCDevice_t* device = get_softiic_inited_handle();
    set_simple_text(&text, device, "hello!");
    on_set_hander_of_line(&text, 1);
    set_simple_text(&text, device, "hello! newline!");
    char meg[40];
    double res = 0;
    AverageADC_Policy p;
    reset_avg_policy(&p);
    p.avg_times = 20;
    p.delay_ms = 10;
    while (1)
    {
        res = fetch_adc_avg_raw_value(&adc_handle, &p);
        snprintf(meg, 40, "adc num fetched: %.3f as Raw", res);
        set_simple_text(&text, device, meg);
        res *= (3.3/4096);
        snprintf(meg, 40, "adc num fetched: %.3f V", res);
        set_newline_text(&text, device, meg);
        system_delay_ms(500);        
        text.operations.clear(&text);
        on_set_hander_of_line(&text, 2);
    }
    return 0;
}
