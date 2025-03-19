#ifndef CCGraphic_Frame_H
#define CCGraphic_Frame_H
#include "ccgraphic/widget/CCGraphic_WidgetBase.h"
#include "ccgraphic/base/CCGraphic_Rectangle/CCGraphic_Rectangle.h"

typedef struct {
    CCGraphicWidgetCommonOperation  operation;
}CCGraphic_Frame_SupportiveOperation;


typedef struct __CCGraphic_Frame
{
    CCGraphic_Rectangle rect;
    CCDevice_t*    handler;
    device_size_t        line_width;
    CCGraphic_Frame_SupportiveOperation operation;
}CCGraphic_Frame;

void CCGraphic_createDeviceFrame(
    CCGraphic_Frame* frame, CCDevice_t* handler,
    device_size_t    line_width, 
    device_size_t margin_width, device_size_t margin_height);



#endif

