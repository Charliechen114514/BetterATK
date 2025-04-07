#include "stm32f1xx.h"
#include "system_clock.h"

/**
 * @brief the delay function for the system clock, delay in us
 * 
 * @param xus 
 */
void system_delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				
	SysTick->VAL = 0x00;					
	SysTick->CTRL = 0x00000005;				
	while(!(SysTick->CTRL & 0x00010000));	
	SysTick->CTRL = 0x00000004;				
}

/**
 * @brief the delay function for the system clock, delay in ms
 * 
 * @param xms 
 */
void system_delay_ms(uint32_t xms)
{
	volatile uint32_t ready_delay = xms;
	while(ready_delay--)
	{
		system_delay_us(1000);
	}
}
