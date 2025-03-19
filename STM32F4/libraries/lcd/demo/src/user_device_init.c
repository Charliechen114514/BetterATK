#include "ccgraphic/device_adapter/device_adapter.h"
#include "lcd/lcd.h"
#include "system_clock.h"

const uint16_t LCD_ColorTable[] = {
    0xFFFF, // COLOR_WHITE
    0x0000, // COLOR_BLACK
    0x001F, // COLOR_BLUE
    0xF81F, // COLOR_BRED
    0xFFE0, // COLOR_GRED
    0x07FF, // COLOR_GBLUE
    0xF800, // COLOR_RED
    0xF81F, // COLOR_MAGENTA
    0x07E0, // COLOR_GREEN
    0x7FFF, // COLOR_CYAN
    0xFFE0, // COLOR_YELLOW
    0xBC40, // COLOR_BROWN
    0xFC07, // COLOR_BRRED
    0x8430, // COLOR_GRAY
    0x01CF, // COLOR_DARKBLUE
    0x7D7C, // COLOR_LIGHTBLUE
    0x5458, // COLOR_GRAYBLUE
    0x841F, // COLOR_LIGHTGREEN
    0xEF5B, // COLOR_LIGHTGRAY
    0xC618, // COLOR_LGRAY
    0xA651, // COLOR_LGRAYBLUE
    0x2B12  // COLOR_LBBLUE
};

#define COLOR_CNT (sizeof(LCD_ColorTable)/sizeof(LCD_ColorTable[0]))

void test_cleared(CCDevice_t* device)
{
    for(uint16_t i = 0; i < 1; i++){
        system_delay_ms(1000);
        set_pen_color(&device->device_property.pen, LCD_ColorTable[i]);
        device->device_funtions.cleared(device);
    }
}
#undef COLOR_CNT

void test_partial_filled(CCDevice_t* device)
{
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_GREEN);
    device->device_funtions.cleared(device);
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_BLUE);
    CCGraphic_Point begin = {
        .x = 0,
        .y = 0
    };
    CCGraphic_Point end = {
        .x = device->device_property.width,
        .y = 40
    };    
    device->device_funtions.filled(device, begin, end);
    begin.x = 0;
    begin.y = device->device_property.height - 40;
    end.y = device->device_property.height;
    device->device_funtions.filled(device, begin, end);
}

void test_draw_points(CCDevice_t* device)
{
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_RED);
    CCGraphic_Point begin = {
        .x = 0,
        .y = 41
    };    
    for(uint16_t i = 0; i < 60; i++){
        device->device_funtions.draw_point(device, begin);
        begin.x++;
        begin.y++;
    }
}


#include "ccgraphic/widget/base/ccgraphic_textitem/ccgraphic_ascii_textitem.h"
void test_draw_ascii(CCDevice_t* device)
{
    CCGraphic_AsciiTextItem item;
    CCGraphic_TextItemProperty p = {
        .backgraound_color  = DEVICE_COLOR_LIGHTGREEN,
        .font_color         = DEVICE_COLOR_BROWN,
        .font_size          = ASCII_FONT_16x08,
        .req_overlaying     = req_overlaying_yes
    };

    CCGraphic_Point begin = {
        .x = 20,
        .y = 100
    };

    CCGraphic_Size size = {
        .width = 50,
        .height = 200
    };

    char* src = "Hello, My friend!";
    CCGraphicWidget_AsciiTextItem_init(
        &item, p, begin, size 
    );

    CCGraphicWidget_AsciiTextItem_setAsciiText(&item, src);
    CCGraphicWidget_AsciiTextItem_drawText(device, &item);
}

#include "ccgraphic/base/CCGraphic_Line/CCGraphic_Line.h"
static void __line(CCDevice_t* device){
    CCGraphic_Line l;
    CCGraphic_Point begin = {
        .x = 70,
        .y = 10
    };
    CCGraphic_Point end = {
        .x = 10,
        .y = 300
    };    
    CCGraphic_init_line(&l, begin, end);
    CCGraphic_draw_line(device, &l);
}

#include "ccgraphic/base/CCGraphic_Rectangle/CCGraphic_Rectangle.h"
static void __rect(CCDevice_t* device){
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_GREEN);
    CCGraphic_Rectangle rect;
    CCGraphic_Point tl = {
        .x = 200,
        .y = 300
    };
    CCGraphic_Point br = {
        .x = 300,
        .y = 400
    };   
    CCGraphic_init_rectangle(&rect, tl, br);
    CCGraphic_drawfilled_rectangle(device, &rect);
}

#include "ccgraphic/base/CCGraphic_Triangle/CCGraphic_Triangle.h"
static void __triangle(CCDevice_t* device)
{
    CCGraphic_Triangle  triangle;
    CCGraphic_Point     p1;
    CCGraphic_Point     p2;
    CCGraphic_Point     p3;

    p1.x = 50;
    p1.y = 10;

    p2.x = 150;
    p2.y = 5;

    p3.x = 80;
    p3.y = 40;

    CCGraphic_init_triangle(&triangle, p1, p3, p2);
    CCGraphic_drawfilled_triangle(device, &triangle);
}

#include "ccgraphic/base/CCGraphic_Circle/CCGraphic_Circle.h"
static void __circle(CCDevice_t* device)
{
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_LBBLUE);
    CCGraphic_Circle c;
    CCGraphic_Point p;
    p.x = 64;
    p.y = 100;
    CCGraphic_init_circle(&c, p, 40);
    CCGraphic_drawfilled_circle(device, &c);

    p.x = 40;
    p.y = 32;
    CCGraphic_init_circle(&c, p, 20);
    CCGraphic_draw_circle(device, &c);
}

void test_draw_base_graphcis(CCDevice_t* device)
{
    __line(device);
    __rect(device);
    __triangle(device);
    __circle(device);
}

#include "ccgraphic/widget/base/CCGraphic_Image/CCGraphic_Image.h"
extern const unsigned char gImage_test[67200];
void test_draw_advanced(CCDevice_t* device)
{
    CCGraphic_Image image;
    CCGraphic_Point tl = {
        .x = 50,
        .y = 50
    };
    CCGraphic_Size size = {
        .height = 240,
        .width = 240
    };
    CCGraphicWidget_init_image(&image, tl, size, (uint8_t*)gImage_test);
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_WHITE);
    CCGraphicWidget_draw_image(device, &image);
}

#include "ccgraphic/sim_app/simple_text.h"
#include <stdio.h>
CCGraphicTextEdit edit;
void test_end(CCDevice_t* device)
{
    set_pen_color(&device->device_property.pen, DEVICE_COLOR_WHITE);
    device->device_funtions.cleared(device);
    char buffer[40];
    snprintf(buffer, 40, "Hello from %d!", 114);
    set_newline_text(&edit, device, buffer);
    snprintf(buffer, 40, "This is charlie's speeking!");
    set_newline_text(&edit, device, buffer);
    while(1);
}

void test_queue(CCDevice_t* device)
{
    test_cleared(device);
    // test_partial_filled(&device);
    test_draw_points(device);
    test_draw_ascii(device);
    test_draw_base_graphcis(device);    
    test_draw_advanced(device);
    test_end(device);
}
 



