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
		.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10,
		.GPIO_Speed = GPIO_Speed_50MHz}};

CCGPIO_ExtiLineInit init1 = {
	.configure_sources = {
		.pinSources = GPIO_PinSource0,
		.PortSources = GPIO_PortSourceGPIOB},
	.exit_line = EXTI_Line0,
	.mode = GPIO_EXTI_FALLING,
	.type = GPIO_EXTI_INTR,
	.IRQSettings = {.line_irq_num = EXTI0_IRQn, .main_preepmt = 1, .sub_preempt = 1}};

CCGPIO_ExtiLineInit init2 = {
	.configure_sources = {
		.pinSources = GPIO_PinSource1,
		.PortSources = GPIO_PortSourceGPIOB},
	.exit_line = EXTI_Line1,
	.mode = GPIO_EXTI_FALLING,
	.type = GPIO_EXTI_INTR,
	.IRQSettings = {.line_irq_num = EXTI1_IRQn, .main_preepmt = 1, .sub_preempt = 2}};

CCGPIO_ExtiLineInit init3 = {
	.configure_sources = {
		.pinSources = GPIO_PinSource10,
		.PortSources = GPIO_PortSourceGPIOB},
	.exit_line = EXTI_Line10,
	.mode = GPIO_EXTI_FALLING,
	.type = GPIO_EXTI_INTR,
	.IRQSettings = {.line_irq_num = EXTI15_10_IRQn, .main_preepmt = 1, .sub_preempt = 3}};

int result = 0;
int delta = 0;

CCDeviceHandler handler;
CCGraphicTextEdit	edit;
#include <stdio.h>

void hookA(CCGPIOType* types){
	if (fetch_pin_status(types) == 0)
	{
		types->pin = GPIO_Pin_1;
		if (fetch_pin_status(types) == 0)
		{
			delta--;
		}
	}
}

void hookB(CCGPIOType* types){
	if (fetch_pin_status(types) == 0)
	{
		types->pin = GPIO_Pin_0;
		if (fetch_pin_status(types) == 0)
		{
			delta++;
		}
	}
}

void hookKey(CCGPIOType* types){
	result = 0;
	delta = 0;
}

int fetch_result(void)
{
	int Temp;
	Temp = delta;
	result = 0;
	return Temp;
}


int main()
{
	oled_soft_iic_setup(&handler);
	configure_nvic_group(NVIC_PriorityGroup_2);

	configure_ccgpio(NULL, &ports);
	configure_gpio_exitline(&init1);
	configure_gpio_exitline(&init2);
	configure_gpio_exitline(&init3);

	CCGPIOType type = {
		.port = GPIOB,
		.pin = GPIO_Pin_0
	};
	register_exitmode(hookA, &type);
	type.pin = GPIO_Pin_1;
	register_exitmode(hookB, &type);
	type.pin = GPIO_Pin_10;
	register_exitmode(hookKey, &type);

	char buffer[20];
	while(1)
	{
		result += fetch_result();
		snprintf(buffer, 20, "Num: %d", result);
		set_simple_text(&edit, &handler, buffer);
		system_delay_ms(100);
	}

}
