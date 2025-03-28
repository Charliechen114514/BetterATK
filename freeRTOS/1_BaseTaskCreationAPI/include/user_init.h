#ifndef USER_INIT_H
#define USER_INIT_H
#include "gpio.h"
#include "ccuart.h"

extern CCUART   uart1;
extern CCGPIOType led0, led1;

void init_bsp_led(void);
void init_uart(void);

#endif