#ifndef SIMPLE_TEXT_H
#define SIMPLE_TEXT_H
#include "ccgraphic/widgets/components/CCGraphic_TextEdit/CCGraphic_TextEdit.h"
void set_simple_text(
    CCGraphicTextEdit* edit, CCDeviceHandler* handler,char* sources);
void set_newline_text(CCGraphicTextEdit* edit, CCDeviceHandler* handler,char* sources);
void reset_to_header(CCGraphicTextEdit* edit);
#endif