#ifndef CCGraphic_Image_H
#define CCGraphic_Image_H
#include "ccgraphic/base/ccgraphic_size.h"
#include "ccgraphic/base/CCGraphic_Point/CCGraphic_Point.h"

typedef struct __CCGraphic_Image{
    CCGraphic_Point point;
    CCGraphic_Size  image_size;
    uint8_t*        sources_register;
}CCGraphic_Image;

void CCGraphicWidget_init_image(
    CCGraphic_Image*    image,
    CCGraphic_Point     tl_point,
    CCGraphic_Size      image_size,
    uint8_t*      sources_register
);

void CCGraphicWidget_draw_image(
    CCDevice_t*    handler,
    CCGraphic_Image*    image
);


#endif