#ifndef CCGRAGHIC_PEN_H
#define CCGRAGHIC_PEN_H
#include "ccgraphic/base/ccgraphic_color.h"
typedef enum {
    SUPPORT_RGB_DRAW    = 1
}CCDevice_PenProperty;

typedef unsigned short pen_property_t;

typedef struct {
    pen_property_t      property_bytes;
    CCGraphicColor      pen_color;
}CCDevice_Pen;

void setup_device_pen(
    CCDevice_Pen*       pen,
    pen_property_t      properties,
    CCGraphicColor      default_color
);

void set_pen_color(
    CCDevice_Pen*       pen,
    CCGraphicColor      color);

#endif