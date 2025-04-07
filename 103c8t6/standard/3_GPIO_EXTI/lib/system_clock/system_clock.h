#ifndef __DELAY_H
#define __DELAY_H
#include <stdint.h>

/**
 * @brief the delay function for the system clock, delay in us
 * 
 * @param xus 
 */
void system_delay_us(uint32_t xus);

/**
 * @brief the delay function for the system clock, delay in ms
 * 
 * @param xms 
 */
void system_delay_ms(uint32_t xms);


#endif
