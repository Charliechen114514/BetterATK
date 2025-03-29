# 从0开始——在PlatformIO下开展STM32单片机的HAL库函数编程指南

## 前言

​	笔者最开始的尝试是在2025年的寒假，准备向PlatformIO迁移HAL库，注意，截止到目前，PlatformIO对HAL库的支持已经非常完善了。一些博客给出的配置可能已经过时，或者因为不太理解PIO的结构导致错误的配置。

​	HAL库因为存在大量的回调函数，导致移植起来相当的困难（笔者稍有的猜测是为了跟CubeMX耦合，当然这个说法大家图一乐就好），漏了东西就会一不小心就会把单片机陷入Infinite Loop了。因此，需要我们做细致的移植。

​	好消息是，我们不需要移植HAL库的代码，PIO那边已经有了代码了，我们需要的是Hook，一部分中断的Hook编写和最基础的时钟初始化的问题（对的，默认使用的是HSI系统），而且我们的HAL库使用的Systicks_Handler更新HAL库的时基。

## 生成HAL库的PIO框架

这里选择好芯片和位置，工程名称后，记得选择框架为STMCubeMX框架。

## 编写时钟初始化

​	第一步，就是修正我们的HSE，这个事情需要根据自己板子的外设决定了，HSE_Value的值需要根据自己板子的外部晶振定义，笔者建议不要修改默认的，自己在上面定义HSE_Value，这样我们使用外部晶振的时候值才准确。

​	下面就是编写时钟驱动了：

> system_clock.h

```c
#ifndef SYSCLK_H
#define SYSCLK_H
#include <stm32f1xx_hal.h>

/*
    This is the clk tree params interface
    if you are lazy in configured, use defaults
*/
void sys_stm32_clock_init(uint32_t plln);

#endif
```

> system_clock.c

```c
#include "system_clock.h"

void sys_stm32_clock_init(uint32_t plln)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    RCC_OscInitTypeDef rcc_osc_init = {0};
    RCC_ClkInitTypeDef rcc_clk_init = {0};

    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;       /* 选择要配置HSE */
    rcc_osc_init.HSEState = RCC_HSE_ON;                         /* 打开HSE */
    rcc_osc_init.HSEPredivValue = RCC_HSE_PREDIV_DIV1;          /* HSE预分频系数 */
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;                     /* 打开PLL */
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;             /* PLL时钟源选择HSE */
    rcc_osc_init.PLL.PLLMUL = plln;                             /* PLL倍频系数 */
    ret = HAL_RCC_OscConfig(&rcc_osc_init);                     /* 初始化 */

    if (ret != HAL_OK)
    {
        while (1);                                              
    }

    /* 选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2*/
    rcc_clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;        /* 设置系统时钟来自PLL */
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;               /* AHB分频系数为1 */
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV2;                /* APB1分频系数为2 */
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;                /* APB2分频系数为1 */
    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_2);  /* 同时设置FLASH延时周期为2WS，也就是3个CPU周期。 */

    if (ret != HAL_OK)
    {
        while (1);                                              
    }
}
```

## 实现Systicks_Handler，完成HAL库的时基更新

```
#include "stm32f1xx_hal.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
}
```

​	就这样就行

## 编写驱动测试

```
#include "stm32f1xx_hal.h"
#include "system_clock.h"

int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOA_CLK_ENABLE();                                 

    gpio_init_struct.Pin = GPIO_PIN_0;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);       /* 初始化LED0引脚 */
    while(1)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
        HAL_Delay(500);
    }
}
```

​	一样的，将PA0介入LED，LED会正确的周期性闪烁。