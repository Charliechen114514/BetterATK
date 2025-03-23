#include "ccgraphic_font.h"

extern const unsigned char ascii12x6_sources[][12];
extern const unsigned char ascii16x8_sources[][16];

unsigned char* _select_fonts_remotely(
    ASCII_FONT_SIZE size, const char ch)
{
    switch (size)
    {
    case ASCII_FONT_12x06:
        return (unsigned char*)(ascii12x6_sources[ch - ' ']);
    case ASCII_FONT_16x08:
        return (unsigned char*)(ascii16x8_sources[ch - ' ']);
    default:
        break;
    }
    return NULL;
}

void _fetch_font_size(
    CCGraphic_Size* size, ASCII_FONT_SIZE acsii_size)
{
    switch (acsii_size)
    {
    case ASCII_FONT_12x06:
        size->height = 12;
        size->width = 6;
        break;
    case ASCII_FONT_16x08:
        size->height = 16;
        size->width = 8;        
        break;
    default:
        break;
    }
}