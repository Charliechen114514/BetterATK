#include "ccgraphic/sim_app/simple_text.h"
CCGraphic_AsciiTextItem item;
static uint8_t  is_inited = 0;

static void __helper_on_set_text(CCGraphicTextEdit* edit, char* sources)
{
    edit->operations.setText(edit, sources);
}

static void __helper_on_newline_text(CCGraphicTextEdit* edit, char* sources)
{
    edit->operations.newLineText(edit, sources);
}

#define SET_TEXT_CONV(SRC) do{ sources = SRC;\
    __helper_on_set_text(edit, sources);}while(0)

#define NEW_LINE_CONV(SRC) do{ sources = SRC;\
    __helper_on_newline_text(edit, sources);}while(0)


static void __pvt_set_up(CCGraphicTextEdit* edit, CCDevice_t* handler)
{
    CCGraphic_Point p;
    p.x = 0;
    p.y = 0;
    CCGraphic_Size acceptablesize = 
        CCGraphicWidget_MaxAcceptable_Size(handler);
    CCGraphic_TextItemProperty property = {
        .backgraound_color = SIMAPP_BK_COLOR,
        .font_color = SIMAPP_FONT_COLOR,
        .req_overlaying = req_overlaying_yes,
        .font_size = ASCII_FONT_16x08
    };
    CCGraphicWidget_AsciiTextItem_init(
        &item, property, p, acceptablesize);
    CCGraphic_init_CCGraphicTextEdit(
        edit, handler, &item
    );
    edit->acquired_stepped_update = 1;
    is_inited = 1;
}


void set_simple_text(
    CCGraphicTextEdit* edit, CCDevice_t* handler,char* sources)
{
    if(!is_inited) __pvt_set_up(edit, handler);
    SET_TEXT_CONV(sources);
}

void set_newline_text(CCGraphicTextEdit* edit, CCDevice_t* handler,char* sources)
{
    if(!is_inited) __pvt_set_up(edit, handler);
    NEW_LINE_CONV(sources);
}