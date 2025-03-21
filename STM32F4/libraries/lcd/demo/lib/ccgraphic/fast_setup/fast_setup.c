#include "fast_setup.h"
#include "lcd/lcd.h"
LCDHandleType   lcd;
CCDevice_t      device;

static void __open_lcd_clk()
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

SoftLCDInitGPIOPack packs = {
    .gpio_clk_open = __open_lcd_clk,
    .gpios = {
        {.pinType = GPIO_PIN_15, .port = GPIOB},
        {.pinType = GPIO_PIN_12, .port = GPIOB},
        {.pinType = GPIO_PIN_11, .port = GPIOB},
        {.pinType = GPIO_PIN_5, .port = GPIOB},
        {.pinType = GPIO_PIN_3, .port = GPIOB},
        {.pinType = GPIO_PIN_13, .port = GPIOB},
        {.pinType = GPIO_PIN_4, .port = GPIOB}}};

LCDSoftTypeHandlePrivate private_handle = {
    .packages_cached = &packs};

void user_lcd_init(LCDHandleType *handle)
{
    init_lcdhandle_soft(handle, LCD_ST7796, &private_handle);
    handle->operations.clear_lcd(handle, COLOR_LBBLUE);
}

CCDevice_t* get_softiic_inited_handle()
{
    user_lcd_init(&lcd);
    init_device(&device, LCD, &lcd);
    return &device;
}