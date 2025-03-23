#ifndef __CCGraphic_Line_H
#define __CCGraphic_Line_H
#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"

typedef struct __CCGraphic_Line{
    CCGraphic_Point p_left;
    CCGraphic_Point p_right;
}CCGraphic_Line;

void CCGraphic_init_line(   CCGraphic_Line* line, 
                            CCGraphic_Point pl, 
                            CCGraphic_Point pr);

void CCGraphic_draw_line(CCDevice_t* handler, CCGraphic_Line* line);


#endif