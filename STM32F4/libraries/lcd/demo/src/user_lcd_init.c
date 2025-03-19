#include "lcd.h"
#include "system_clock.h"

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

static const uint16_t LCD_ColorTable[] = {
    0xFFFF, // COLOR_WHITE
    0x0000, // COLOR_BLACK
    0x001F, // COLOR_BLUE
    0xF81F, // COLOR_BRED
    0xFFE0, // COLOR_GRED
    0x07FF, // COLOR_GBLUE
    0xF800, // COLOR_RED
    0xF81F, // COLOR_MAGENTA
    0x07E0, // COLOR_GREEN
    0x7FFF, // COLOR_CYAN
    0xFFE0, // COLOR_YELLOW
    0xBC40, // COLOR_BROWN
    0xFC07, // COLOR_BRRED
    0x8430, // COLOR_GRAY
    0x01CF, // COLOR_DARKBLUE
    0x7D7C, // COLOR_LIGHTBLUE
    0x5458, // COLOR_GRAYBLUE
    0x841F, // COLOR_LIGHTGREEN
    0xEF5B, // COLOR_LIGHTGRAY
    0xC618, // COLOR_LGRAY
    0xA651, // COLOR_LGRAYBLUE
    0x2B12  // COLOR_LBBLUE
};

#define COLOR_CNT (sizeof(LCD_ColorTable)/sizeof(LCD_ColorTable[0]))

void user_test_lcd_screen_color(LCDHandleType *handle)
{
    for(uint16_t i = 0; i < COLOR_CNT; i++){
        system_delay_ms(1000);
        handle->operations.clear_lcd(handle, LCD_ColorTable[i]);
    }
}

#undef COLOR_CNT