#ifndef CCGraphic_WidgetBase_H
#define CCGraphic_WidgetBase_H
#include "ccgraphic/CCGraphic_common.h"
#include "ccgraphic/widgets/common/CCGraphic_Size/CCGraphic_Size.h"

CCGraphic_Size  CCGraphicWidget_MaxAcceptable_Size(
    CCDeviceHandler* device_handle
);

typedef void* CCgraphicWidgetBase;

/* update requist functions */
typedef void(*Update)(CCgraphicWidgetBase);
typedef void(*Hide)(CCgraphicWidgetBase);
typedef void(*Show)(CCgraphicWidgetBase);


typedef struct{
    Update      update;
    Hide        hide;
    Show        show;
}CCGraphicWidgetCommonOperation;


#endif