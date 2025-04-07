#include "gpio_exti.h"
#include "system_clock.h"
#include "Graphic/sim_app/simple_text.h"
#include "Graphic/fast_setup/fast_setup.h"
#include "Graphic/CCGraphic_device_adapter.h"
static void __common_clk(CCGPIOType* t)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

CCGPIO_InitType ports = {
	.clk_open = __common_clk,
	.post_init = 0,
	.port = GPIOB,
	.initer = {
		.GPIO_Mode = GPIO_Mode_IPU,
		.GPIO_Pin = GPIO_Pin_0,
		.GPIO_Speed = GPIO_Speed_50MHz}};

CCGPIO_ExtiLineInit init1 = {
	.configure_sources = {
		.pinSources = GPIO_PinSource0,
		.PortSources = GPIO_PortSourceGPIOB},
	.exit_line = EXTI_Line0,
	.mode = GPIO_EXTI_FALLING,
	.type = GPIO_EXTI_INTR,
	.IRQSettings = {.line_irq_num = EXTI0_IRQn, .main_preepmt = 1, .sub_preempt = 1}};

int result = 0;
int delta = 0;

CCDeviceHandler handler;
CCGraphicTextEdit	edit;
#include <stdio.h>

void hookA(CCGPIOType* types){
	if (fetch_pin_status(types) == 0)
	{
		while (fetch_pin_status(types) == 1);
		result = 1;
	}
}


int main()
{
	oled_soft_iic_setup(&handler);
	configure_nvic_group(NVIC_PriorityGroup_2);

	configure_ccgpio(NULL, &ports);
	configure_gpio_exitline(&init1);

	CCGPIOType type = {
		.port = GPIOB,
		.pin = GPIO_Pin_0
	};
	register_exitmode(hookA, &type);
	const char* text;
	while(1)
	{
		text = "Light is off!";
		if(result){
			result = 0;
			text = "sensor triggure once!";
			set_simple_text(&edit, &handler, text);
			system_delay_ms(1000);
		}
		set_simple_text(&edit, &handler, text);
		system_delay_ms(100);
	}
}
