/*
    Main Entry for the key detections
*/
#include "ccuart.h"
#include "system_clock.h"
#include "gpio.h"
#include "lcd.h"
#include "ccgraphic/device_adapter/device_adapter.h"

void user_lcd_init(LCDHandleType* handle);
void test_queue(CCDevice_t* device);

int main(void)
{
    HAL_Init();
    system_clock_initAs168MHz(USE_HSE_AS_SYSCLK);
    LCDHandleType   lcd;
    CCDevice_t      device;
    user_lcd_init(&lcd);
    init_device(&device, LCD, &lcd);
    while(1){
        test_queue(&device);
    }

    return 0;
}
