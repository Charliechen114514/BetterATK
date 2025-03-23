#include "ccgraphic/widget/base/ccgraphic_textitem/ccgraphic_ascii_textitem.h"

#include <string.h>
void CCGraphicWidget_AsciiTextItem_init(
    CCGraphic_AsciiTextItem *item,
    CCGraphic_TextItemProperty property,
    CCGraphic_Point tl_point,
    CCGraphic_Size accept_size)
{
    item->property = property;
    item->sources_display = "";
    item->tl_point = tl_point;
    item->texthandleSize = accept_size;
    item->indexed_point = tl_point;
}

void CCGraphicWidget_AsciiTextItem_setAsciiText(
    CCGraphic_AsciiTextItem *item,
    char *text)
{
    item->sources_display = text;
}

void CCGraphicWidget_AsciiTextItem_setIndexedPoint(
    CCGraphic_AsciiTextItem *item,
    CCGraphic_Point *p)
{
    item->indexed_point = *p;
}

static void __draw_char(
    CCDevice_t *device,
    CCGraphic_Size *font_size,
    CCGraphic_TextItemProperty *property,
    CCGraphic_Point *tl_p, unsigned char *char_src)
{
    if(!char_src) return;
    unsigned char temp = 0;
    color_t cached_color = device->device_property.pen.pen_color;
    CCGraphic_Point br_p = {
        .x = tl_p->x + font_size->width - 1,
        .y = tl_p->y + font_size->height - 1};
    device->device_funtions.set_draw_mappings(device, *tl_p, br_p);
    switch (property->req_overlaying)
    {
    case req_overlaying_yes:
    {
        for (unsigned char i = 0; i < font_size->height; i++)
        {
            temp = char_src[i];
            for (unsigned char j = 0; j < font_size->width; j++)
            {
                device->device_funtions.transmit_rawdata(
                    device, (temp & 0x01 ? property->font_color : property->backgraound_color));
                temp >>= 1;
            }
        }
    }
    break;
    case req_overlaying_no:
    {
        set_pen_color(&device->device_property.pen, property->font_color);
        CCGraphic_Point each;
        for (unsigned char i = 0; i < font_size->height; i++)
        {
            temp = char_src[i];
            for (unsigned char j = 0; j < font_size->width; j++)
            {
                if (temp & 0x01)
                {
                    each.x = tl_p->x + j;
                    each.y = tl_p->y + i;
                    device->device_funtions.draw_point(
                        device, each);
                }
                temp >>= 1;
            }
        }
        set_pen_color(&device->device_property.pen, cached_color);
    }
    break;
    default:
        break;
    }
    CCGraphic_Point begin = {
        .x = 0,
        .y = 0
    };
    CCGraphic_Point end = {
        .x = device->device_property.width - 1,
        .y = device->device_property.height - 1
    };    
    device->device_funtions.set_draw_mappings(device, begin, end);
}

void CCGraphicWidget_AsciiTextItem_relocate(
    CCGraphic_AsciiTextItem*    item,
    CCGraphic_Point             tl_point,
    CCGraphic_Size              textHandleSize)
{
    item->tl_point          = tl_point;
    item->texthandleSize    = textHandleSize;
}


static CCGraphic_Point inline __pvt_fetch_valid_final_point(
    CCDevice_t* device_handle,
    CCGraphic_Size* size, CCGraphic_Point* tl
) 
{
    CCGraphic_Point br;
    /* check if the br valid */
    ;
    br.x = tl->x + size->width;
    br.y = tl->y + size->height;
    if(device_handle->device_property.width < br.x){
        br.x = device_handle->device_property.width;
    }
    if(device_handle->device_property.height < br.y){
        br.y = device_handle->device_property.height;
    }
    return br;
}

static unsigned char inline __pvt_should_be_next_line(
    CCDevice_t*                device_handle,
    CCGraphic_Point*                brpoint,
    CCGraphic_Point* cur_draw_p, 
    unsigned short font_width)
{
    return cur_draw_p->x + 
        (short)(1.1 * font_width) >= brpoint->x;
}


void CCGraphicWidget_AsciiTextItem_drawText(
    CCDevice_t *device,
    CCGraphic_AsciiTextItem *item)
{
    if(strcmp(item->sources_display, "") == 0){
        return;
    }

    CCGraphic_Point draw_tl_point;

    draw_tl_point = item->indexed_point;

    const ASCII_FONT_SIZE font_size = item->property.font_size;
    CCGraphic_Size size;
    _fetch_font_size(&size, font_size);
    const device_size_t font_width = size.width;
    const device_size_t font_height = size.height;

    CCGraphic_Point br = __pvt_fetch_valid_final_point(
        device, &(item->texthandleSize), &(item->tl_point) 
    );

    unsigned char offseterx = 0;
    unsigned char  offsetery = 0;
    for(unsigned char  i = 0; 
        item->sources_display[i] != '\0'; 
        i++)
    {
        draw_tl_point.x = 
                item->indexed_point.x + offseterx * font_width;
        draw_tl_point.y = 
                item->indexed_point.y + offsetery * font_height;
        __draw_char(
            device, &size, &item->property, 
            &draw_tl_point, 
            _select_fonts_remotely(
                item->property.font_size, item->sources_display[i])
        );
                // precheck if we need next line draw :)
        if(__pvt_should_be_next_line(
            device,&br, &draw_tl_point, font_width))
        {
            offseterx = 0;
            // meet the max one, switch to the next line
            offsetery ++;
            item->indexed_point.x = item->tl_point.x;
        }else{
            offseterx ++;
        }
    }
    item->indexed_point = draw_tl_point;
    item->indexed_point.x += font_width; 
}

CCGraphic_Point  CCGraphicWidget_drawAsciiTextItem_with_finPoint(
    CCDevice_t*                 device,
    CCGraphic_AsciiTextItem*    item,
    AppendMethod                method
)
{
    if(strcmp(item->sources_display, "") == 0){
        return item->indexed_point;
    }

    CCGraphic_Point draw_tl_point;

    draw_tl_point = item->indexed_point;

    const ASCII_FONT_SIZE font_size = item->property.font_size;
    CCGraphic_Size size;
    _fetch_font_size(&size, font_size);
    const device_size_t font_width = size.width;
    const device_size_t font_height = size.height;

    CCGraphic_Point br = __pvt_fetch_valid_final_point(
        device, &(item->texthandleSize), &(item->tl_point) 
    );

    unsigned char offseterx = 0;
    unsigned char  offsetery = 0;
    for(unsigned char  i = 0; 
        item->sources_display[i] != '\0'; 
        i++)
    {
        draw_tl_point.x = 
                item->indexed_point.x + offseterx * font_width;
        draw_tl_point.y = 
                item->indexed_point.y + offsetery * font_height;
        __draw_char(
            device, &size, &item->property, 
            &draw_tl_point, 
            _select_fonts_remotely(
                item->property.font_size, item->sources_display[i])
        );
                // precheck if we need next line draw :)
        if(__pvt_should_be_next_line(
            device,&br, &draw_tl_point, font_width))
        {
            offseterx = 0;
            // meet the max one, switch to the next line
            offsetery ++;
            item->indexed_point.x = item->tl_point.x;
        }else{
            offseterx ++;
        }
    }
    item->indexed_point = draw_tl_point;
    item->indexed_point.x += font_width; 
    switch(method)
    {
        case CCGraphic_AsciiTextItem_AppendNextLine:
            draw_tl_point.x = item->tl_point.x;
            draw_tl_point.y += font_height;
            break;
        case CCGraphic_AsciiTextItem_AppendContinously:
            draw_tl_point.x += font_width;
            break;
        default:
            break;
    }
    return draw_tl_point;
}

CCGraphic_Point CCGraphicWidget_AsciiTextItem_on_append_point(CCGraphic_AsciiTextItem* item)
{
    return item->indexed_point;
}

CCGraphic_Point CCGraphicWidget_AsciiTextItem_on_newLine_point(CCGraphic_AsciiTextItem* item)
{
    CCGraphic_Point draw_tl_point;
    draw_tl_point.x = item->tl_point.x;
    CCGraphic_Size size;
    _fetch_font_size(&size, item->property.font_size);
    draw_tl_point.y = item->indexed_point.y + size.height;
    return draw_tl_point;    
}