#ifndef CCGraphic_Frame_H
#define CCGraphic_Frame_H
#include "ccgraphic/widgets/common/CCGraphic_WidgetBase.h"
#include "ccgraphic/base/CCGraphic_Rectangle/CCGraphic_Rectangle.h"
typedef struct __CCGraphic_Frame CCGraphic_Frame;
typedef struct {
    CCGraphicWidgetCommonOperation  operation;
    void(*draw_self)(CCGraphic_Frame*);
}CCGraphic_Frame_SupportiveOperation;


typedef struct __CCGraphic_Frame
{
    CCGraphic_Rectangle rect;
    CCDeviceHandler*    handler;
    SizeBaseType        line_width;
    CCGraphic_Frame_SupportiveOperation operation;
}CCGraphic_Frame;

void CCGraphic_createDeviceFrame(
    CCGraphic_Frame* frame, CCDeviceHandler* handler,
    SizeBaseType    line_width, 
    SizeBaseType margin_width, SizeBaseType margin_height);



#endif

