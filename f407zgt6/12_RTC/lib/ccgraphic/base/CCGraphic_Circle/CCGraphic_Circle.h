#ifndef __CCGraphic_Circle_H
#define __CCGraphic_Circle_H
#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"
#include "ccgraphic/device_adapter/device_adapter.h"

typedef struct __CCGraphic_Circle
{
    
    CCGraphic_Point             center;
    device_size_t               radius;
}CCGraphic_Circle;

void CCGraphic_init_circle(CCGraphic_Circle* circle, CCGraphic_Point c, uint8_t radius);
void CCGraphic_draw_circle(CCDevice_t* handler, CCGraphic_Circle* circle);
void CCGraphic_drawfilled_circle(CCDevice_t* handler, CCGraphic_Circle* circle);
#endif