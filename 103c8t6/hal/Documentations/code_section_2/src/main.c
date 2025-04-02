#include "stm32f1xx_hal.h"
#include "system_clock.h"

void init_led()
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOA_CLK_ENABLE();                                 

    gpio_init_struct.Pin = GPIO_PIN_1;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);       /* 初始化LED0引脚 */
}

void init_bsp_key()
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE();                                 

    gpio_init_struct.Pin = GPIO_PIN_1;                   
    gpio_init_struct.Mode = GPIO_MODE_INPUT;            
    gpio_init_struct.Pull = GPIO_PULLUP;                    
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);
}

static uint8_t isKeyPressed()
{
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET;
}

static void led_flippings()
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    HAL_Delay(500);
}


int main()
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    init_led();
    init_bsp_key();
    while(1)
    {
        if(isKeyPressed()){
            led_flippings();
        }
    }
}