#include "CCGraphic_Circle.h"
#include "ccgraphic/device_adapter/device_adapter.h"

void CCGraphic_init_circle(
    CCGraphic_Circle* circle, CCGraphic_Point c, uint8_t radius)
{
    circle->center = c;
    circle->radius = radius;
}

#define DRAW_OFFSET_POINT(point, offsetx, offsety) \
    do { \
        point.x = circle->center.x + (offsetx); \
        point.y = circle->center.y + (offsety); \
        CCGraphic_draw_point(handler, &point);}while(0)

void CCGraphic_draw_circle(
    CCDevice_t* handler, CCGraphic_Circle* circle)
{
	/*参考文档：https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
	/*参考教程：https://www.bilibili.com/video/BV1VM4y1u7wJ*/
    CCGraphic_Point p;
    int16_t d = 3 - circle->radius / 4;
    int16_t x = 0;
    int16_t y = circle->radius;

    DRAW_OFFSET_POINT(p, x, y);
    DRAW_OFFSET_POINT(p, -x, -y);
    DRAW_OFFSET_POINT(p, y, x);
    DRAW_OFFSET_POINT(p, -y, -x);

    while(x < y)
    {
        x++;
        if(d < 0){ d += 2 * x + 1;}
        else {y--; d += 2 * (x - y) + 1;}
        DRAW_OFFSET_POINT(p, x, y);
        DRAW_OFFSET_POINT(p, y, x);
        DRAW_OFFSET_POINT(p, -x, -y);
        DRAW_OFFSET_POINT(p, -y, -x);
        DRAW_OFFSET_POINT(p, x, -y);
        DRAW_OFFSET_POINT(p, y, -x);
        DRAW_OFFSET_POINT(p, -x, y);
        DRAW_OFFSET_POINT(p, -y, x);            
    }
}

void CCGraphic_drawfilled_circle(CCDevice_t* handler, CCGraphic_Circle* circle)
{
    CCGraphic_Point p;
    int16_t d = 1 - circle->radius;
    int16_t x = 0;
    int16_t y = circle->radius;

    DRAW_OFFSET_POINT(p, x, y);
    DRAW_OFFSET_POINT(p, -x, -y);
    DRAW_OFFSET_POINT(p, y, x);
    DRAW_OFFSET_POINT(p, -y, -x);

    for(int16_t i = -y; i < y; i++)
        DRAW_OFFSET_POINT(p, 0, i);

    while(x < y)
    {
        x++;
        if(d < 0){ d += 2 * x + 1;}
        else {y--; d += 2 * (x - y) + 1;}
        DRAW_OFFSET_POINT(p, x, y);
        DRAW_OFFSET_POINT(p, y, x);
        DRAW_OFFSET_POINT(p, -x, -y);
        DRAW_OFFSET_POINT(p, -y, -x);
        DRAW_OFFSET_POINT(p, x, -y);
        DRAW_OFFSET_POINT(p, y, -x);
        DRAW_OFFSET_POINT(p, -x, y);
        DRAW_OFFSET_POINT(p, -y, x);   
        for(int16_t i = -y; i < y; i++)
        {
            DRAW_OFFSET_POINT(p, x, i);
            DRAW_OFFSET_POINT(p, -x, i);  
        }
        for(int16_t i = -x; i < x; i++)
        {
            DRAW_OFFSET_POINT(p, y, i);
            DRAW_OFFSET_POINT(p, -y, i);  
        }              
    }    
}

#undef DRAW_OFFSET_POINT