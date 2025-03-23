#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"
#include "ccgraphic/device_adapter/device_adapter.h"
void CCGraphic_draw_point(
    CCDevice_t* handler, CCGraphic_Point* point)
{
    handler->device_funtions.draw_point(handler, *point);
}