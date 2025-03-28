#include "type_adapter/type_adapter.h"

#ifdef ACCEPT_ST7796
#include "st7796/lcdst7796_property.h"
#endif

void setup_decrete_lcd(
    LCDHandleType *handle,
    LCD_DeviceType type)
{
    switch (type)
    {
#ifdef ACCEPT_ST7796
    case LCD_ST7796:
        // init the lcd property infos
        on_init_st7796property(&handle->lcd_property);
        // filled the lcd operations
        on_filled_st7796commands(handle);
        // set up the inner init
        on_inter_setup_st7796soft(handle);
        break;
#endif
    default:
        break;
    }
}
