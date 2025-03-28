#ifndef CCGRAPHIC_FONT_H
#define CCGRAPHIC_FONT_H
#include "ccgraphic/base/ccgraphic_size.h"
typedef enum {

    ASCII_FONT_12x06,
    
    ASCII_FONT_16x08

}ASCII_FONT_SIZE;

unsigned char* _select_fonts_remotely(ASCII_FONT_SIZE size, const char ch);
void _fetch_font_size(
    CCGraphic_Size* size, ASCII_FONT_SIZE acsii_size);


#endif