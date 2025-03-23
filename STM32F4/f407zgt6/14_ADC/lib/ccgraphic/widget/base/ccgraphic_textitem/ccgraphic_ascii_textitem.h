#ifndef CCGRAPHIC_ASCII_TEXTITEM_H
#define CCGRAPHIC_ASCII_TEXTITEM_H
#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"
#include "ccgraphic/base/ccgraphic_color.h"
#include "ccgraphic/base/ccgraphic_size.h"
#include "ccgraphic/base/CCGraphic_Font/ccgraphic_font.h"
#include "ccgraphic/device_adapter/device_adapter.h"

typedef enum
{
    req_overlaying_no,
    req_overlaying_yes
} req_overlaying_t;

typedef struct
{
    CCGraphicColor font_color;
    CCGraphicColor backgraound_color;
    ASCII_FONT_SIZE font_size;
    req_overlaying_t req_overlaying;
} CCGraphic_TextItemProperty;

typedef struct
{
    // sources are set to waiting display
    char *sources_display;
    CCGraphic_Point tl_point;
    CCGraphic_Point indexed_point;
    CCGraphic_Size texthandleSize;
    CCGraphic_TextItemProperty property;
} CCGraphic_AsciiTextItem;

void CCGraphicWidget_AsciiTextItem_init(
    CCGraphic_AsciiTextItem *item,
    CCGraphic_TextItemProperty property,
    CCGraphic_Point tl_point,
    CCGraphic_Size accept_size);

void CCGraphicWidget_AsciiTextItem_relocate(
    CCGraphic_AsciiTextItem *item,
    CCGraphic_Point tl_point,
    CCGraphic_Size textHandleSize);

void CCGraphicWidget_AsciiTextItem_setAsciiText(
    CCGraphic_AsciiTextItem *item,
    char *text);

void CCGraphicWidget_AsciiTextItem_setIndexedPoint(
    CCGraphic_AsciiTextItem *item,
    CCGraphic_Point *p);

void CCGraphicWidget_AsciiTextItem_drawText(
    CCDevice_t *device,
    CCGraphic_AsciiTextItem *item);

typedef enum
{
    /*
        This enum promised that, the point next using in set the
        tl point will directly append the text, this can be using in
        the upper level widget to implement the appendText()
    */
    CCGraphic_AsciiTextItem_AppendContinously,
    /*
        This enum will directly tells the point that the newline should be
        drawn, which locates the next line
    */
    CCGraphic_AsciiTextItem_AppendNextLine,
    /*
        do nothing, using in self defined behaviour, this will returns the
        final char's drawing tl_pont
    */
    CCGraphic_AsciiTextItem_LastDrawPoint
} AppendMethod;

CCGraphic_Point CCGraphicWidget_drawAsciiTextItem_with_finPoint(
    CCDevice_t *device_handle,
    CCGraphic_AsciiTextItem *item,
    AppendMethod method);

/* on provide the nextline draw point for the item */
CCGraphic_Point CCGraphicWidget_AsciiTextItem_on_newLine_point(CCGraphic_AsciiTextItem *item);
/* on provide the appensive draw point for the item */
CCGraphic_Point CCGraphicWidget_AsciiTextItem_on_append_point(CCGraphic_AsciiTextItem *item);
#endif