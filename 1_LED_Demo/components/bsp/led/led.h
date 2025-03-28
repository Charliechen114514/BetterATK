
#ifndef __LED_H
#define __LED_H
#include "driver/gpio.h"

#define LED0_GPIO_PIN    GPIO_NUM_1    

void led_init(void);    
void led_toggle_pin(void);
#endif
