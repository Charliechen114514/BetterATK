#ifndef LCD_COLOR_TYPE_H
#define LCD_COLOR_TYPE_H

typedef enum {
    COLOR_WHITE       = 0xFFFF,
    COLOR_BLACK       = 0x0000,
    COLOR_BLUE        = 0x001F,
    COLOR_BRED        = 0xF81F,
    COLOR_GRED        = 0xFFE0,
    COLOR_GBLUE       = 0x07FF,
    COLOR_RED         = 0xF800,
    COLOR_MAGENTA     = 0xF81F,
    COLOR_GREEN       = 0x07E0,
    COLOR_CYAN        = 0x7FFF,
    COLOR_YELLOW      = 0xFFE0,
    COLOR_BROWN       = 0xBC40,
    COLOR_BRRED       = 0xFC07,
    COLOR_GRAY        = 0x8430,

    // GUI 颜色
    COLOR_DARKBLUE    = 0x01CF, // 深蓝色
    COLOR_LIGHTBLUE   = 0x7D7C, // 浅蓝色
    COLOR_GRAYBLUE    = 0x5458, // 灰蓝色

    COLOR_LIGHTGREEN  = 0x841F, // 浅绿色
    COLOR_LIGHTGRAY   = 0xEF5B, // 浅灰色 (PANEL)
    COLOR_LGRAY       = 0xC618, // 浅灰色 (窗体背景色)

    COLOR_LGRAYBLUE   = 0xA651, // 浅灰蓝色 (中间层颜色)
    COLOR_LBBLUE      = 0x2B12  // 浅棕蓝色 (选择条目的反色)
} LCD_Color;


#endif