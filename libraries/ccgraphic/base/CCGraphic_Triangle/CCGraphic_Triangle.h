#ifndef CCGraphic_Triangle_H
#define CCGraphic_Triangle_H

#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"

typedef struct __CCGraphic_Triangle
{
    CCGraphic_Point     p1;
    CCGraphic_Point     p2;
    CCGraphic_Point     p3;
}CCGraphic_Triangle;


void CCGraphic_init_triangle(
    CCGraphic_Triangle* triangle, 
    CCGraphic_Point     p1,
    CCGraphic_Point     p2,
    CCGraphic_Point     p3
);

void CCGraphic_draw_triangle(
    CCDevice_t*    handle,
    CCGraphic_Triangle* triangle
);

void CCGraphic_drawfilled_triangle(
    CCDevice_t*    handle,
    CCGraphic_Triangle* triangle
);

#endif