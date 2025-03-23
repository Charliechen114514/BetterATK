#ifndef USER_INIT_H
#define USER_INIT_H
#include "gpio.h"
#include "ccuart.h"
#include "key.h"
extern CCUART   uart1;
extern CCGPIOType led0, led1;
extern KeyPort key1, key2;

void init_bsp_led(void);
void init_uart(void);
void init_bsp_key(void);

#endif