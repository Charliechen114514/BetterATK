#ifndef LCD_ST7796_PROPERTY_H
#define LCD_ST7796_PROPERTY_H
#include "lcd.h"


#define ST7796_WIDTH        (320)
#define ST7796_HEIGHT       (480)

void on_init_st7796property(LCD_BaseProperty*   property);
void on_filled_st7796commands(LCDHandleType*  handle);
void on_inter_setup_st7796soft(LCDHandleType* handle);
#endif