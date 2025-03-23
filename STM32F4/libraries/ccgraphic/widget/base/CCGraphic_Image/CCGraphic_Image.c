#include "CCGraphic_Image.h"
#include "ccgraphic/device_adapter/device_adapter.h"
void CCGraphicWidget_init_image(
    CCGraphic_Image *image,
    CCGraphic_Point tl_point,
    CCGraphic_Size image_size,
    uint8_t *sources_register)
{
    image->image_size = image_size;
    image->point = tl_point;
    image->sources_register = sources_register;
}

void CCGraphicWidget_draw_image(
    CCDevice_t *handler,
    CCGraphic_Image *image)
{
    if (!image->sources_register)
        return;
    handler->device_funtions.area_drawer(
        handler, image->point,
        image->image_size, image->sources_register);
}