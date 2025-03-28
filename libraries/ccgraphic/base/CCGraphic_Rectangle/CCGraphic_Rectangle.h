#ifndef CCGraphic_Rectangle_H
#define CCGraphic_Rectangle_H


#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"

typedef struct __CCGraphic_Rectangle{
    CCGraphic_Point         top_left;
    CCGraphic_Point         bottom_right;
}CCGraphic_Rectangle;

void CCGraphic_init_rectangle(
    CCGraphic_Rectangle* rect, CCGraphic_Point tl, CCGraphic_Point br);

void CCGraphic_draw_rectangle(
    CCDevice_t* handler, CCGraphic_Rectangle* rect);

void CCGraphic_drawfilled_rectangle(
    CCDevice_t* handler, CCGraphic_Rectangle* rect);



#endif