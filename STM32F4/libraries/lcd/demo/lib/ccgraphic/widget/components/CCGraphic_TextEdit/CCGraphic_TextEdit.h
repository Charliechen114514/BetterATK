#ifndef CCGraphic_TextEdit_H
#define CCGraphic_TextEdit_H
#include "ccgraphic/widget/CCGraphic_WidgetBase.h"
#include "ccgraphic/base/CCGraphic_Point/CCGraphic_Point.h"
#include "ccgraphic/widget/base/ccgraphic_textitem/ccgraphic_ascii_textitem.h"
#include "ccgraphic/base/CCGraphic_Font/ccgraphic_font.h"
typedef struct __CCGraphicTextEdit CCGraphicTextEdit;

typedef struct{
    CCGraphicWidgetCommonOperation  operation;
    void (*appendText)(CCGraphicTextEdit*, char* appendee);
    void (*setText)(CCGraphicTextEdit*, char* text);
    void (*newLineText)(CCGraphicTextEdit*, char* text);
    void (*clear)(CCGraphicTextEdit*);
    void (*relocate)(CCGraphicTextEdit*, CCGraphic_Point p, CCGraphic_Size size);
}CCGraphicTextEdit_SupportiveOperations;

typedef struct __CCGraphicTextEdit
{
    uint8_t                                 acquired_stepped_update;
    CCDevice_t*                        borrowed_device;
    CCGraphicTextEdit_SupportiveOperations  operations;
    CCGraphic_AsciiTextItem*                handle;
}CCGraphicTextEdit;

void CCGraphic_init_CCGraphicTextEdit(
    CCGraphicTextEdit*          text_self,
    CCDevice_t*            handler,
    CCGraphic_AsciiTextItem*    inited
);


#endif