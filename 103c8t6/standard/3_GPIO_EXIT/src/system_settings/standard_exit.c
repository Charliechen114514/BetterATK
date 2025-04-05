#include "gpio_exti.h"

void EXTI0_IRQHandler(void)
{
    HookPrecheck check = {
        .exti_line = EXTI_Line0};
    CCGPIOType type = {
        .port = GPIOB,
        .pin = GPIO_Pin_0};
    do_gpio_exti_hook(&check, &type);
}

void EXTI1_IRQHandler(void)
{
    HookPrecheck check = {
        .exti_line = EXTI_Line1};
    CCGPIOType type = {
        .port = GPIOB,
        .pin = GPIO_Pin_1};
    do_gpio_exti_hook(&check, &type);
}

void EXTI15_10_IRQHandler(void)
{
    HookPrecheck check = {
        .exti_line = EXTI_Line10};
    CCGPIOType type = {
        .port = GPIOB,
        .pin = GPIO_Pin_10};
    do_gpio_exti_hook(&check, &type);
}