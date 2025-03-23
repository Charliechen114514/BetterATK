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
extern InputCapture input_capture;

volatile uint8_t capture_Cnt = 0;
uint32_t capture_Buf[2] = {0};
char buffer[50];
void callback_ic(InputCapture *input_capture)
{
    switch (capture_Cnt)
    {
    case 1:
        capture_Buf[0] = ic_captured_value(input_capture);          
        config_capture_triggure(input_capture, IC_CAP_FALLING); 
        capture_Cnt++;
        break;
    case 2:
        capture_Buf[1] = ic_captured_value(input_capture); 
        stop_inputCapture(input_capture);                        
        capture_Cnt++;
    }
}

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    led_setup();
    setup_usart();
    init_input_capture();
    register_ic_callback(&input_capture, callback_ic);
    uint32_t high_level_time = 0;
    while (1)
    {
        switch (capture_Cnt)
        {
        case 0:
            capture_Cnt++;
            config_capture_triggure(&input_capture, IC_CAP_RISING);
            start_inputCapture(&input_capture);
            break;
        case 3:
            high_level_time = capture_Buf[1] - capture_Buf[0];
            snprintf(buffer, 50, "high_level_maintains: %ld\n", high_level_time);
            send_message(&uart1, buffer);
            capture_Cnt = 0; // reset 0
            break;
        }
    }

    return 0;
}
