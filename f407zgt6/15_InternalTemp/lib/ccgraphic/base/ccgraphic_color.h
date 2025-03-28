#ifndef CCGRAPHIC_COLOR_H
#define CCGRAPHIC_COLOR_H

typedef enum {
    DEVICE_COLOR_WHITE       = 0xFFFF,
    DEVICE_COLOR_BLACK       = 0x0000,
    DEVICE_COLOR_BLUE        = 0x001F,
    DEVICE_COLOR_BRED        = 0xF81F,
    DEVICE_COLOR_GRED        = 0xFFE0,
    DEVICE_COLOR_GBLUE       = 0x07FF,
    DEVICE_COLOR_RED         = 0xF800,
    DEVICE_COLOR_MAGENTA     = 0xF81F,
    DEVICE_COLOR_GREEN       = 0x07E0,
    DEVICE_COLOR_CYAN        = 0x7FFF,
    DEVICE_COLOR_YELLOW      = 0xFFE0,
    DEVICE_COLOR_BROWN       = 0xBC40,
    DEVICE_COLOR_BRRED       = 0xFC07,
    DEVICE_COLOR_GRAY        = 0x8430,

    // GUI 颜色
    DEVICE_COLOR_DARKBLUE    = 0x01CF, // 深蓝色
    DEVICE_COLOR_LIGHTBLUE   = 0x7D7C, // 浅蓝色
    DEVICE_COLOR_GRAYBLUE    = 0x5458, // 灰蓝色

    DEVICE_COLOR_LIGHTGREEN  = 0x841F, // 浅绿色
    DEVICE_COLOR_LIGHTGRAY   = 0xEF5B, // 浅灰色 (PANEL)
    DEVICE_COLOR_LGRAY       = 0xC618, // 浅灰色 (窗体背景色)

    DEVICE_COLOR_LGRAYBLUE   = 0xA651, // 浅灰蓝色 (中间层颜色)
    DEVICE_COLOR_LBBLUE      = 0x2B12  // 浅棕蓝色 (选择条目的反色)
} CCGraphicColor ;

#endif
