#include "ccgraphic/widgets/components/CCGraphic_Clock/CCGraphic_Clock.h"
#include "ccgraphic/widgets/components/CCGraphic_Frame/CCGraphic_Frame.h"
#include "time/time_ui.h"
#include "time/time_utils.h"
#include "ccgraphic/sim_app/simple_text.h"
#include "rtc.h"
#include "ccgraphic/CCGraphic_device_adapter.h"
#include <stdio.h>
extern CCRTClock rtc_clk;
CCGraphic_AsciiTextItem time_shown_internal;
CCGraphicTextEdit timeShown;
CCGrapghic_Clock clock;
uint8_t clock_init_flag;

static void __init_clock(CCGrapghic_Clock *__clock, CCDeviceHandler *handler)
{
    CCGraphic_Circle circle = {
        .center = {
            .x = 32,
            .y = 32},
        .radius = 27};
    initCCGraphic_Clock(
        __clock, handler, &circle);
}

static void __init_edit_item(CCDeviceHandler *handler)
{
    CCGraphic_Point p;
    p.x = 64;
    p.y = 15;
    CCGraphic_Size acceptablesize = {
        .width = 60,
        .height = 45
    };
    CCGraphicWidget_init_AsciiTextItem(
        &time_shown_internal, p, acceptablesize, ASCII_6x8
    );
    CCGraphic_init_CCGraphicTextEdit(
        &timeShown, handler, &time_shown_internal
    );
    timeShown.acquired_stepped_update = 0;
}

static void __create_base_menu(CCDeviceHandler *handler)
{
    CCGraphic_Frame frame;
    CCGraphic_createDeviceFrame(&frame, handler, 1, 2, 2);
    frame.operation.draw_self(&frame);
}

static void on_set_clock_status(CCGrapghic_Clock *clk, CCDateTime *dateTime)
{
    uint8_t hour = dateTime->time.Hours % 12;
    CCGraphic_Time time = {
        .hour = hour,
        .minutes = dateTime->time.Minutes,
        .second = dateTime->time.Seconds};
    clock.op.updateTime(clk, &time);
}

static void draw_text_part(
    CCDeviceHandler *handler, CCDateTime *times)
{
    char buffer_disp[15];
    snprintf(buffer_disp, 10, "Year:%d", (times->date.Year + 2000));
    set_simple_text(&timeShown, handler, buffer_disp);
    char month[5];
    on_fetch_month_engstr(times->date.Month - 1, month, 5);
    snprintf(buffer_disp, 10, "Mon:%s",month);
    set_newline_text(&timeShown, handler, buffer_disp);
    snprintf(buffer_disp, 10, "Date:%d",times->date.Date);
    set_newline_text(&timeShown, handler, buffer_disp);
    snprintf(buffer_disp, 12, "Hours:%d", (times->time.Hours));
    set_newline_text(&timeShown, handler, buffer_disp);
    snprintf(buffer_disp, 15, "Time:%d:%d", (times->time.Minutes), (times->time.Seconds));
    set_newline_text(&timeShown, handler, buffer_disp);  
    reset_to_header(&timeShown);
}

void turn_to_clock_page(
    CCDeviceHandler *handler,
    TimeInfoPackage *timeInfoPackage)
{
    if (clock_init_flag == 0)
    {
        // init done!
        __init_clock(&clock, handler);
        __init_edit_item(handler);
        clock_init_flag = 1;
    }
    handler->operations.clear_device_function(handler);
    CCDateTime datetime;
    get_rtc_dateTime(&rtc_clk, &datetime);
    __create_base_menu(handler);
    on_set_clock_status(&clock, &datetime);
    draw_text_part(handler, &datetime);
    handler->operations.update_device_function(handler);
}