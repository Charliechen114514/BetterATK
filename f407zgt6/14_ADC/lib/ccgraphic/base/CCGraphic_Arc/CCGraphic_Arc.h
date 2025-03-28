#ifndef CCGraphic_Arc_H
#define CCGraphic_Arc_H

#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"

typedef struct __CCGraphic_Arc{
    CCGraphic_Point     center;
    device_size_t       radius;
    int16_t             start_degree;
    int16_t             end_degree;
}CCGraphic_Arc;

void CCGraphic_init_CCGraphic_Arc(
    CCGraphic_Arc*      handle,
    CCGraphic_Point     center,
    device_size_t       radius,
    int16_t             start_degree,
    int16_t             end_degree  
);

void CCGraphic_draw_arc(
    CCDevice_t* handler,
    CCGraphic_Arc* handle
);

void CCGraphic_drawfilled_arc(
    CCDevice_t* handler,
    CCGraphic_Arc* handle
);

#endif