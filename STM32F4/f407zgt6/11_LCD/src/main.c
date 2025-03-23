/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
#include "input_capture.h"
#include <stdio.h>
extern CCGPIOType led0, led1;
extern CCUART uart1;
CCGraphicTextEdit text;

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();

    CCDevice_t* lcd_handle = get_softiic_inited_handle();
    set_simple_text(&text, lcd_handle, "hello!");

    while (1)
    {

    }

    return 0;
}
