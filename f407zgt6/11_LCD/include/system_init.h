#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "ccgraphic/fast_setup/fast_setup.h"
#include "ccgraphic/sim_app/simple_text.h"
void led_setup();

void setup_usart();

void key_init();

void timer_init();

void pwm_init();

void init_input_capture();
#endif