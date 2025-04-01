#include "system_clock.h"
#include "stm32f10x.h"

void init_led()
{
	GPIO_InitTypeDef gpio_init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio_init);
}

int main(void)
{
	init_led();
	
	while (1)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, 1);
		system_delay_ms(500);
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, 0);
		system_delay_ms(500);
	}
}
