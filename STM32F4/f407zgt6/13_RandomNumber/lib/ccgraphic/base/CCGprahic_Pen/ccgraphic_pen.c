#include "ccgraphic_pen.h"

void setup_device_pen(
    CCDevice_Pen*       pen,
    pen_property_t      properties,
    CCGraphicColor      default_color)
{
    pen->property_bytes = properties;
    if(!(properties & SUPPORT_RGB_DRAW)){
        pen->pen_color = DEVICE_COLOR_BLACK;
    }else{
        pen->pen_color = default_color;
    }
}

void set_pen_color(
    CCDevice_Pen*       pen,
    CCGraphicColor      color)
{
    if(!(pen->property_bytes & SUPPORT_RGB_DRAW)){
        return;
    }
    pen->pen_color = color;
}