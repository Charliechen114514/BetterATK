#include "device_lcd.h"

static void filled_lcd(
    CCDevice_t* blank, CCGraphic_Point begin, CCGraphic_Point end)
{
    LCDHandleType* lcd_handle = (LCDHandleType*)blank->device_handle;
    LCDRawPoint begin_p = {
        .x = begin.x,
        .y = begin.y
    };
    LCDRawPoint end_p = {
        .x = end.x,
        .y = end.y       
    };
    lcd_handle->operations.filled_lcd(
        lcd_handle, begin_p, end_p, blank->device_property.pen.pen_color);
}

static void cleared_lcd(CCDevice_t* blank)
{
    LCDHandleType* lcd_handle = (LCDHandleType*)blank->device_handle;
    lcd_handle->operations.clear_lcd(lcd_handle, blank->device_property.pen.pen_color);
}

static void draw_point(CCDevice_t* blank, CCGraphic_Point point)
{
    LCDHandleType* lcd_handle = (LCDHandleType*)blank->device_handle;
    LCDRawPoint p = {
        .x = point.x,
        .y = point.y
    };
    lcd_handle->operations.drawPoint(lcd_handle, p, blank->device_property.pen.pen_color);
}

static void set_cursor(CCDevice_t* blank, CCGraphic_Point point)
{
    LCDHandleType* lcd_handle = (LCDHandleType*)blank->device_handle;
    LCDRawPoint p = {
        .x = point.x,
        .y = point.y
    };
    lcd_handle->operations.set_cursor(lcd_handle, p);
}

static void send_raw(CCDevice_t* blank, uint16_t data){
    LCDHandleType* lcd_handle = (LCDHandleType*)blank->device_handle;
    lcd_handle->operations.send_data(lcd_handle, data);
}

static void set_draw_mappings(CCDevice_t* blank, 
    CCGraphic_Point begin, CCGraphic_Point end)
{
    LCDHandleType* lcd_handle = (LCDHandleType*)blank->device_handle;
    LCDRawPoint begin_p = {
        .x = begin.x,
        .y = begin.y
    };
    LCDRawPoint end_p = {
        .x = end.x,
        .y = end.y       
    };
    lcd_handle->operations.set_mapping_window(lcd_handle, begin_p, end_p);
}

static void area_drawer(CCDevice_t *device, CCGraphic_Point point, CCGraphic_Size size, uint8_t* src)
{
    LCDHandleType* lcd_handle = (LCDHandleType*)device->device_handle;
    LCDRawPoint begin_p = {
        .x = point.x,
        .y = point.y
    };

    lcd_handle->operations.drawArea(lcd_handle, begin_p, size.width, size.height, src);
}

static void setup_properties(CCDevice_t* blank, LCDHandleType* lcd_handle)
{
    setup_device_pen(&blank->device_property.pen, SUPPORT_RGB_DRAW, COLOR_BLACK);
    blank->device_property.height = lcd_handle->lcd_property.lcd_height;
    blank->device_property.width = lcd_handle->lcd_property.lcd_width;
}

void setup_as_lcd(CCDevice_t* blank, LCDHandleType* lcd_handle)
{
    setup_properties(blank, lcd_handle);
    blank->device_handle = lcd_handle;
    blank->device_funtions.filled = filled_lcd;
    blank->device_funtions.cleared = cleared_lcd;
    blank->device_funtions.draw_point = draw_point;
    blank->device_funtions.setcursor = set_cursor;
    blank->device_funtions.area_drawer = area_drawer;
    blank->device_funtions.transmit_rawdata = send_raw;
    blank->device_funtions.set_draw_mappings = set_draw_mappings;
}