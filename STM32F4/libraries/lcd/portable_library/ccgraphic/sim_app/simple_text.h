#ifndef SIMPLE_TEXT_H
#define SIMPLE_TEXT_H
#include "ccgraphic/widget/components/CCGraphic_TextEdit/CCGraphic_TextEdit.h"

#define SIMAPP_FONT_COLOR      (DEVICE_COLOR_BLACK)
#define SIMAPP_BK_COLOR        (DEVICE_COLOR_WHITE)

void set_simple_text(
    CCGraphicTextEdit* edit, CCDevice_t* handler,char* sources);
void set_newline_text(CCGraphicTextEdit* edit, CCDevice_t* handler,char* sources);
#endif