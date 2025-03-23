#include "CCGraphic_WidgetBase.h"
#include "ccgraphic/device_adapter/device_adapter.h"


CCGraphic_Size  CCGraphicWidget_MaxAcceptable_Size(
    CCDevice_t* device_handle)
{
    CCGraphic_Size s = {
        .height = device_handle->device_property.height,
        .width = device_handle->device_property.width
    };
    return s;
}