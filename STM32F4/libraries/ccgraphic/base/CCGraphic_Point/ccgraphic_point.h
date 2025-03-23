#ifndef CCGRAPHIC_POINT_H
#define CCGRAPHIC_POINT_H
#include "ccgraphic/ccgraphic_types.h"

typedef struct __ccgraphic_point{
    device_size_t           x;
    device_size_t           y;
}CCGraphic_Point;

void CCGraphic_draw_point(
    CCDevice_t* handler, CCGraphic_Point* point);

#endif