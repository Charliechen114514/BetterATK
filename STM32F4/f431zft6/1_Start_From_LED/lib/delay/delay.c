#include "delay.h"

static uint64_t __sysclk_freq;
void init_delay(void)
{
    __sysclk_freq = HAL_RCC_GetSysClockFreq() / 1000000L;
}

void delay_us(uint64_t us)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt = 0;
	uint32_t reload = SysTick->LOAD;    /*LOAD的值*/
	ticks = us * __sysclk_freq;             /*需要的节拍数*/

	told = SysTick->VAL;                /*刚进入时的计数器值*/
	while(1)
	{
		tnow = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told)
			{
				tcnt += told - tnow; /*注意一下SYSTICK是一个递减的计数器*/ 
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			if(tcnt >= ticks)
			{
				break;            /*时间超过/等于要延时的时间，则退出*/
			}
		}
	}
}



void delay_ms(uint16_t ms)
{
    for(uint16_t i = 0; i < ms; i++){
        delay_us(1000);
    }
}
