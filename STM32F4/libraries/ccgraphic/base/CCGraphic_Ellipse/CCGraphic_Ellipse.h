#ifndef CCGraphic_Ellipse_H
#define CCGraphic_Ellipse_H

#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"

typedef struct __CCGraphic_Ellipse{
    CCGraphic_Point                 center;
    device_size_t                   X_Radius;
    device_size_t                   Y_Radius;  
}CCGraphic_Ellipse;

void CCGraphic_init_ellipse(
    CCGraphic_Ellipse*          handle, 
    CCGraphic_Point             center,
    device_size_t               X_Radius,
    device_size_t               Y_Radius 
);

void CCGraphic_draw_ellipse(
    CCDevice_t* handler,
    CCGraphic_Ellipse* ellipse
);

void CCGraphic_drawfilled_ellipse(
    CCDevice_t* handler,
    CCGraphic_Ellipse* ellipse
);


#endif