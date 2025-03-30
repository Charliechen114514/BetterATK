#include "gpio.h"
#include "system_clock.h"

static void __open_clk(CCGPIOType* type)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

static void __post_init(CCGPIOType* type)
{
	set_ccgpio_pinstate(type, CCGPIO_HIGH);
}

CCGPIO_InitType	initer = {
	.clk_open = __open_clk,
	.post_init = __post_init,
	.port = GPIOC,
	.initer = {
		.GPIO_Mode = GPIO_Mode_Out_PP,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_Pin = GPIO_Pin_13
	}
};


int main(void)
{
	CCGPIOType	type;
	configure_ccgpio(&type, &initer);
	
	while (1)
	{
		reverse_ccgpio_pinstate(&type);
		system_delay_ms(500);
	}
}
