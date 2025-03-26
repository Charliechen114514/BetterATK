#include "ccgraphic/CCGraphic_common.h"

#define ENABLE 1

#if ENABLE

/* size is 16 x 16 */
const uint8_t test_image[] = {
	0xFF,0x01,0x81,0x81,0x81,0xFD,0x89,0x91,0xA1,0xC1,0xFD,0x81,0x81,0x81,0x01,0xFF,
	0xFF,0x80,0x80,0x80,0x80,0x9F,0x88,0x84,0x82,0x81,0x9F,0x80,0x80,0x80,0x80,0xFF,
};

const uint8_t settingsImage[] = {
0x00, 0x00, 0x80, 0x70, 0x10, 0x90, 0x58, 0x44, 0x4c, 0x90, 0x10, 0x10, 0xb0, 0x80, 0x00, 0x00, 0x00, 0x01, 0x02, 0x1e, 0x10, 0x11, 0x12, 0x24, 
0x24, 0x13, 0x10, 0x10, 0x0a, 0x03, 0x00, 0x00, 
};

const uint8_t helpImage[] = {
0x00, 0xc0, 0xf0, 0xf8, 0xdc, 0x8e, 0x86, 0xe6, 0x66, 0x06, 0x0e, 0x8c, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x3f, 0x7f, 0x7f, 0x64, 
0x64, 0x7e, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x00, 
};

const uint8_t quitImage[] = {
0x00, 0x00, 0x00, 0xf0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x60, 0x60, 0x40, 0x46, 
0x46, 0x46, 0x06, 0x06, 0x1f, 0x0f, 0x06, 0x00, 
};

const uint8_t playImage[] = {
0x00, 0xfe, 0xfc, 0xf8, 0xf0, 0xe8, 0xd8, 0xb0, 0x70, 0x60, 0x00, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x13, 0x09, 
0x0c, 0x04, 0x00, 0x03, 0x01, 0x01, 0x00, 0x00, 
};

#endif

#undef ENABLE


