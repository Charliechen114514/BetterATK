#ifndef CCGraphic_WidgetBase_H
#define CCGraphic_WidgetBase_H

#include "ccgraphic/base/ccgraphic_size.h"

CCGraphic_Size  CCGraphicWidget_MaxAcceptable_Size(
    CCDevice_t* device_handle
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