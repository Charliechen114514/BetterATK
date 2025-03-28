/*
    Main Entry for the key detections
*/

#include "system_clock.h"
#include "system_init.h"
#include "gpio.h"
#include "ccuart.h"
#include <stdio.h>
#include "adc.h"
#include "Test/Test_Global/test_global.h"
extern CCGPIOType led0, led1;
extern CCUART uart1;
extern CCADC_Handle    adc_handle;
CCGraphicTextEdit text;

void SysTick_Handler(void)
{
    HAL_IncTick();
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSI_AS_SYSCLK);
    led_setup();
    setup_usart();
    test_graphic_softspi_functionalities();
    while (1)
    {
    }
    return 0;
}
