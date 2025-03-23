#include "ccgraphic/widget/components/CCGraphic_Clock/CCGraphic_Clock.h"
#include "ccgraphic/widget/components/CCGraphic_Clock/CCGraphic_Clock_Settings.h"
#include "ccgraphic/widget/base/ccgraphic_textitem/ccgraphic_ascii_textitem.h"
#include "ccgraphic/device_adapter/device_adapter.h"
#include "ccgraphic/base/CCGraphic_Font/ccgraphic_font.h"
#include <string.h>
#include <math.h>
static char* MAJOR_SHOW[4] = {"12", "3", "6", "9"};
static CCGraphic_AsciiTextItem text_items[4];

static void __pvt_draw_tickal(
    CCDevice_t* h, CCGraphic_Point center, int angle, device_size_t radius, device_size_t length)
{
    const double K = angle * (3.1415926535L) / 180;
    const double xK = cos(K);
    const double yK = sin(K);
    CCGraphic_Point p;
    for(device_size_t i = radius - length;i < radius; i++)
    {
        p.x = center.x + xK * i;
        p.y = center.y + yK * i;
        CCGraphic_draw_point(h, &p);
    }    
}


static void __draw_angle_line(CCDevice_t* h, CCGraphic_Point center, int angle, device_size_t length)
{
    const double K = angle * (3.1415926535L) / 180;
    const double xK = cos(K);
    const double yK = sin(K);
    CCGraphic_Point p;
    for(device_size_t i = 0;i < length; i++)
    {
        p.x = center.x + xK * i;
        p.y = center.y + yK * i;
        CCGraphic_draw_point(h, &p);
    }
}


static void __pvt_clean(CCGrapghic_Clock* clk){
    CCGraphic_Circle circle = clk->handling_circle;
    CCGraphic_Point tl = {
        .x = circle.center.x - circle.radius,
        .y = circle.center.y - circle.radius
    };
    CCGraphic_Point br = {
        .x = circle.center.x + circle.radius,
        .y = circle.center.y + circle.radius
    };    
    clk->borrowed_device->device_funtions.filled(
        clk->borrowed_device,
        tl, br);
}

static void __pvt_draw(CCGrapghic_Clock* clk)
{
    // draw main
    CCGraphic_draw_circle(clk->borrowed_device, &clk->handling_circle);

    // draw text
    for(uint8_t i = 0; i < 4; i++)
    {
        CCGraphicWidget_AsciiTextItem_drawText(
            clk->borrowed_device, &text_items[i]);
        
    }

    // draw centors
    CCGraphic_Circle circle;
    circle.center = clk->handling_circle.center;
    circle.radius = CLOCK_CENTER_CIRCLE_R;
    CCGraphic_drawfilled_circle(clk->borrowed_device, &circle);
    const uint16_t sec_len = clk->handling_circle.radius - 2;
    const uint16_t min_len = sec_len * 0.9;
    const uint16_t hour_len = sec_len * 0.6;
    // draw tickel
    __draw_angle_line(clk->borrowed_device, circle.center, clk->current_time.second * 6 - 90, sec_len);
    __draw_angle_line(clk->borrowed_device, circle.center, clk->current_time.minutes * 6 - 90, min_len);
    __draw_angle_line(clk->borrowed_device, circle.center, clk->current_time.hour * 30 - 90, hour_len);

    // draw_tickels
    for(uint16_t i = 0; i < 12; i++){
        if(i % 3 == 0) continue;
        __pvt_draw_tickal(clk->borrowed_device, circle.center, 30 * i, clk->handling_circle.radius, 3);
    }

}

static void __pvt_update(CCGrapghic_Clock* clk){}

static void __on_reinit_text_items_locations(CCGrapghic_Clock* clk)
{
    CCGraphic_Point center = clk->handling_circle.center;
    CCGraphic_Size  font_size;
    _fetch_font_size(&font_size, CLOCK_TEXT_SIZE);
    device_size_t   radius = clk->handling_circle.radius; 
    const uint8_t   equal_for_font_offset_w = font_size.width / 2;
    const uint8_t   equal_for_font_offset_h = font_size.height / 2;

    const CCGraphic_Point tl_points[4] = {
        {   center.x - equal_for_font_offset_w * strlen(MAJOR_SHOW[0]), 
            center.y - radius + equal_for_font_offset_h},
        {   center.x + radius - 2.2 * equal_for_font_offset_w,
            center.y - equal_for_font_offset_h},
        {   center.x - equal_for_font_offset_w,
            center.y + radius - 2.2 * equal_for_font_offset_h},
        {   center.x - radius + equal_for_font_offset_w,
            center.y - equal_for_font_offset_h}
    };

    // top one
    CCGraphic_TextItemProperty p = {
        .backgraound_color  = BK_COLOR,
        .font_color         = FONT_COLOR,
        .font_size          = CLOCK_TEXT_SIZE,
        .req_overlaying     = req_overlaying_yes
    };
    for(uint8_t i = 0; i < 4; i++){
        CCGraphicWidget_AsciiTextItem_init(&text_items[i],
            p, tl_points[i], font_size);
        if(i == 0) text_items->texthandleSize.width = 4 * font_size.width;
        else text_items->texthandleSize.width = 2 * font_size.width;
        CCGraphicWidget_AsciiTextItem_setAsciiText(&text_items[i], MAJOR_SHOW[i]);
    }
}

static void __update_time(CCGrapghic_Clock* clk, CCGraphic_Time* time)
{
    clk->current_time = *time;
    __pvt_clean(clk);
    __pvt_draw(clk);
    __pvt_update(clk);
}

void initCCGraphic_Clock(
    CCGrapghic_Clock* clk,CCDevice_t* device,  CCGraphic_Circle* circle)
{
    clk->handling_circle = *circle;
    clk->borrowed_device = device;
    clk->major_tickel_length    = CLOCK_MAJOR_TICKEL_LENGTH;
    clk->minor_tickel_length    = CLOCK_MINOR_TICKEL_LENGTH;
    clk->current_time.hour = 0;
    clk->current_time.minutes = 0;
    clk->current_time.hour = 0;
    clk->op.operations.show = (Show)__pvt_draw;
    clk->op.operations.hide = (Hide)__pvt_clean;
    clk->op.operations.update = (Update)__pvt_update;
    clk->op.updateTime = __update_time;
    __on_reinit_text_items_locations(clk);
}


