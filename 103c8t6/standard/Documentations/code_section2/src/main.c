#include "system_clock.h"
#include "stm32f10x.h"

void init_led()
{
	GPIO_InitTypeDef gpio_init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &gpio_init);
}

void init_bsp_key()
{
	GPIO_InitTypeDef gpio_init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init.GPIO_Mode = GPIO_Mode_IPU;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &gpio_init);	
}

static void __do_reverse_gpio()
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, 1);
	system_delay_ms(500);
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, 0);
	system_delay_ms(500);
}

static uint8_t isKeyPressed()
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_RESET;
}


int main(void)
{
	init_led();
	init_bsp_key();
	while (1)
	{
		if(isKeyPressed()){
			__do_reverse_gpio();
		}
	}
}
