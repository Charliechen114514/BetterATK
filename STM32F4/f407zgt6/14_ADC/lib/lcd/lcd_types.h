#ifndef LCD_TYPES_H
#define LCD_TYPES_H

#include "lcd_compile_config.h"

typedef enum {
    LCD_Direction_0,        // show as normal
    LCD_Direction_90,
    LCD_Direction_180,
    LCD_Direction_270
}LCD_Directions;

typedef enum {
    LCD_BackLightStatus_Off,
    LCD_BackLightStatus_On
}LCD_BackLightStatus;

typedef struct __lcd_base_property
{
    lcd_size_t lcd_width;
    lcd_size_t lcd_height;
    lcd_id_t id;              
    lcd_direction_t dir;
} LCD_BaseProperty;

typedef struct __lcdraw_point{
    lcd_size_t  x;
    lcd_size_t  y;
}LCDRawPoint;


#endif