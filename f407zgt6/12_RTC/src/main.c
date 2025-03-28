/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include "rtc.h"
#include <stdio.h>
extern CCGPIOType led0, led1;
extern CCUART uart1;
extern CCRTClock   rtc_clk;
CCGraphicTextEdit text;


int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    init_rtc();
    config_wakeup_rtc(&rtc_clk, RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0);
    RTC_TimeTypeDef times;
    times.Hours = 3;
    times.Minutes = 43;
    times.Seconds = 0;
    config_alarm(&rtc_clk, 7,&times);
    CCDevice_t* lcd_handle = get_softiic_inited_handle();
    char buffer[50];
    CCDateTime getter;
    while (1)
    {
        set_simple_text(&text, lcd_handle, "hello, world");
        set_newline_text(&text, lcd_handle, "hello, world2!");
        get_rtc_dateTime(&rtc_clk, &getter);
        on_provide_printable_datetime(&getter, buffer, 50, DATE_ONLY);
        set_newline_text(&text, lcd_handle, buffer);
        on_provide_printable_datetime(&getter, buffer, 50, TIME_ONLY);
        set_newline_text(&text, lcd_handle, buffer);        
        reset_to_header(&text);
        system_delay_ms(300);
    }

    return 0;
}
