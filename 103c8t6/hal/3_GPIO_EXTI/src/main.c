#include "system_clock.h"
#include "gpio_exti.h"
#include "Graphic/fast_setup/fast_setup.h"
#include "Graphic/sim_app/simple_text.h"

static void gpio_clk_open()
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

CCGPIO_InitType ports = {
    .clk_open = gpio_clk_open,
    .post_init = NULL,
    .initer = {
        .Mode = GPIO_MODE_IT_FALLING,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH,
        .Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10
    },
    .port = GPIOB
};

int result = 0;
int delta = 0;
void hookA(CCGPIOType* types){
	if (fetch_pin_status(types) == 0)
	{
		types->pin = GPIO_PIN_1;
		if (fetch_pin_status(types) == 0)
		{
			delta--;
		}
	}
}

void hookB(CCGPIOType* types){
	if (fetch_pin_status(types) == 0)
	{
		types->pin = GPIO_PIN_0;
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

CCGPIO_ExtiLineInit init1 = {
    .callbackHook = hookA,
    .interrupt_settings = {
        .irq = EXTI0_IRQn,
        .preempt_level = 1,
        .sub_level = 1
    },
    .which_gpio = {
        .port = GPIOB,
        .pin = GPIO_PIN_0
    }
};

CCGPIO_ExtiLineInit init2 = {
    .callbackHook = hookB,
    .interrupt_settings = {
        .irq = EXTI1_IRQn,
        .preempt_level = 1,
        .sub_level = 2
    },
    .which_gpio = {
        .port = GPIOB,
        .pin = GPIO_PIN_1
    }
};

CCGPIO_ExtiLineInit init3 = {
    .callbackHook = hookKey,
    .interrupt_settings = {
        .irq = EXTI15_10_IRQn,
        .preempt_level = 1,
        .sub_level = 3
    },
    .which_gpio = {
        .port = GPIOB,
        .pin = GPIO_PIN_10
    }
};

int fetch_result(void)
{
	int Temp;
	Temp = delta;
	result = 0;
	return Temp;
}


CCDeviceHandler handler;
CCGraphicTextEdit   edit;
#include <stdio.h>
int main()
{
    HAL_Init();
    system_clock_init();
    configure_ccgpio(NULL, &ports);

    configure_gpio_exitline(&init1);
    configure_gpio_exitline(&init2);
    configure_gpio_exitline(&init3);

    oled_soft_iic_setup(&handler);
    char buffer[20];
    while(1)
    {
        result += fetch_result();
        snprintf(buffer, 20, "Num: %d", result);
        set_simple_text(&edit, &handler, buffer);
        system_delay_ms(200);
    }
}