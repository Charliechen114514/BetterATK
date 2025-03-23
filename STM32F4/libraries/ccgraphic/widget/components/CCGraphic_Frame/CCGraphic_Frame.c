#include "CCGraphic_Frame.h"
#include "ccgraphic/widget/CCGraphic_WidgetBase.h"
#include "ccgraphic/device_adapter/device_adapter.h"
static void __pvt_update(CCGraphic_Frame* frame)
{
    
}

static void __pvt_hide(CCGraphic_Frame* frame)
{
    // clear left
    CCGraphic_Point tl = frame->rect.top_left;
    CCGraphic_Point br = {
        .x = frame->rect.top_left.x + frame->line_width + 1,
        .y = frame->rect.top_left.y = frame->rect.bottom_right.y - frame->rect.top_left.y
    };
    frame->handler->device_funtions.filled(frame->handler, tl , br);
    // clear top
    tl.x = frame->rect.top_left.x;
    tl.y = frame->rect.top_left.y;
    br.x = tl.x + frame->rect.bottom_right.x - frame->rect.top_left.x;
    br.y = tl.y + frame->line_width + 1;
    frame->handler->device_funtions.filled(frame->handler, tl ,br); 
    // clear right
    tl.x = frame->rect.bottom_right.x - frame->line_width;
    tl.y = frame->rect.top_left.y;
    br.x = frame->rect.bottom_right.x;
    br.y = tl.y + frame->rect.bottom_right.y - frame->rect.top_left.y + 1;
    frame->handler->device_funtions.filled(
        frame->handler, tl, br);
    // clear bottom
    tl.x = frame->rect.top_left.x;
    tl.y = frame->rect.bottom_right.y - frame->line_width;
    br.x = frame->rect.bottom_right.x + 1;
    br.y = tl.y + frame->line_width;
    frame->handler->device_funtions.filled(
        frame->handler,tl,br);     
    __pvt_update(frame);
}

static void __pvt_show(CCGraphic_Frame* frame)
{
    // clear left
    CCGraphic_Rectangle tmp;
    tmp = frame->rect;

    for(device_size_t i = 0; i < frame->line_width; i++)
    {
        tmp.top_left.x +=1;
        tmp.top_left.y +=1;
        tmp.bottom_right.x -=1;
        tmp.bottom_right.y -=1;
        CCGraphic_draw_rectangle(frame->handler, &tmp);
    }
    __pvt_update(frame);
}

void CCGraphic_createDeviceFrame(
    CCGraphic_Frame* frame, CCDevice_t* handler, device_size_t line_width, 
    device_size_t margin_width, device_size_t margin_height)
{
    frame->handler = handler;
    CCGraphic_Size  size = CCGraphicWidget_MaxAcceptable_Size(handler);

    if (margin_width + line_width > size.width / 2 || 
        margin_height + line_width> size.height / 2)
    {
        return;
    }
    
    frame->line_width = line_width;

    frame->rect.top_left.x = margin_width;
    frame->rect.top_left.y = margin_height;

    frame->rect.bottom_right.x = size.width     - margin_width;
    frame->rect.bottom_right.y = size.height    -  margin_height;
    frame->operation.operation.update = (Update)__pvt_update;
    frame->operation.operation.show = (Show)__pvt_show;
    frame->operation.operation.hide = (Hide)__pvt_hide;
}

