#include "system_clock.h"
#include "spin_locker/spin_locker.h"
#include "Graphic/fast_setup/fast_setup.h"
#include "Graphic/sim_app/simple_text.h"
#include <stdio.h>
CCDeviceHandler handler;
CCGraphicTextEdit   edit;
int result = 0;
int delta = 0;
char buffer[20];
int fetch_result(void)
{
	int temp = delta;
	result = 0;
	return temp;
}

void update_text(char* status)
{
    result += fetch_result();
    set_simple_text(&edit, &handler, status);
    snprintf(buffer, 20, "Num: %d", result);
    set_newline_text(&edit, &handler, buffer);
    reset_to_start(&edit);
}


void hookLeft(void){
    delta--;
    update_text("You spin left!");
}

void hookRight(void){
    delta++;
    update_text("You spin right!");
}

void hookKey(void){
	result = 0;
	delta = 0;
    update_text("You press keys!");
}


static void gpio_clk_open()
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

CCSpinLockerHandleInitType  ccspinlockertypes = {
    .hooks = {hookLeft, hookRight, hookKey},
    .info = {
        .internal_infos = {
            {
                .open_clk = gpio_clk_open,
                .irq_settings = {
                    .irq_lines = EXTI0_IRQn,
                    .main_preepmt = 1,
                    .sub_preempt = 1
                },
                .types = {
                    .port = GPIOB,
                    .pin = GPIO_PIN_0
                }
            },
            {
                .open_clk = gpio_clk_open,
                .irq_settings = {
                    .irq_lines = EXTI1_IRQn,
                    .main_preepmt = 1,
                    .sub_preempt = 2
                },
                .types = {
                    .port = GPIOB,
                    .pin = GPIO_PIN_1
                }
            },
            {
                .open_clk = gpio_clk_open,
                .irq_settings = {
                    .irq_lines = EXTI15_10_IRQn,
                    .main_preepmt = 1,
                    .sub_preempt = 3
                },
                .types = {
                    .port = GPIOB,
                    .pin = GPIO_PIN_10
                }                
            }
        } 
    }
};

int main()
{
    HAL_Init();
    system_clock_init();
    CCSpinLockerHandle handle;
    initCCSpinLockerHandle(&handle, &ccspinlockertypes);
    oled_soft_iic_setup(&handler);
    
    while(1)
    {
        system_delay_ms(200);
    }
}