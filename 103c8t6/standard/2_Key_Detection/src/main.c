#include "key.h"
#include "system_clock.h"
static void __open_clk(CCGPIOType* type)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

static void __post_init(CCGPIOType* type)
{
	set_ccgpio_pinstate(type, CCGPIO_LOW);
}

static void open_key_clk(CCGPIOType* handle){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

static uint8_t is_key_pressed(KeyHandle* handle){
	uint8_t res = (GPIO_ReadInputDataBit(handle->type.port, handle->type.pin) == 1);
	return res;
}

int main(void)
{
	CCGPIOType	type;
	CCGPIO_InitType	initer = {
		.clk_open = __open_clk,
		.post_init = __post_init,
		.port = GPIOA,
		.initer = {
			.GPIO_Mode = GPIO_Mode_Out_PP,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Pin = GPIO_Pin_1
		}
	};
	configure_ccgpio(&type, &initer);
	KeyHandle handle = {
		.func_detect = is_key_pressed,
		.type = {
			.pin = GPIO_Pin_1,
			.port = GPIOB
		}
	};

	init_bsp_key(&handle, open_key_clk);

	while (1)
	{
		if(isKeyPressed(&handle)){
			reverse_ccgpio_pinstate(&type);
 			system_delay_ms(500);
		}
	}
}
