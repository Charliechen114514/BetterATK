#ifndef DEVICE_ADAPTER_H
#define DEVICE_ADAPTER_H
#include "ccgraphic/base/CCGraphic_Point/ccgraphic_point.h"
#include "ccgraphic/base/CCGprahic_Pen/ccgraphic_pen.h"
#include "ccgraphic/base/ccgraphic_size.h"
#include "ccgraphic/ccgraphic_types.h"
typedef void *GeneralDeviceType;
typedef struct __basedevice CCDevice_t;
typedef enum
{
    LCD
} DeviceType;

typedef void (*Filled)(CCDevice_t *device, CCGraphic_Point begin, CCGraphic_Point end);
typedef void (*Cleared)(CCDevice_t *device);
typedef void (*SetCursor)(CCDevice_t *device, CCGraphic_Point point);
typedef void (*DrawPoint)(CCDevice_t *device, CCGraphic_Point point);
typedef void (*Transmit_rawdata)(CCDevice_t *device, unsigned short data);
typedef void (*SetDrawingMappings)(
    CCDevice_t *device, CCGraphic_Point begin, CCGraphic_Point end);
typedef void (*DrawArea)(
    CCDevice_t *device, CCGraphic_Point point, CCGraphic_Size size, uint8_t* src);

typedef struct __device_base
{
    Filled              filled;
    Cleared             cleared;
    SetCursor           setcursor;
    DrawPoint           draw_point;
    Transmit_rawdata    transmit_rawdata;
    SetDrawingMappings  set_draw_mappings;
    DrawArea            area_drawer;
} DeviceFunctions;

typedef struct __device_property
{
    CCDevice_Pen        pen;
    device_size_t       width;
    device_size_t       height;
}DeviceProperty;

typedef struct __basedevice
{
    DeviceType device_type;
    GeneralDeviceType device_handle;
    DeviceFunctions device_funtions;
    DeviceProperty  device_property;
} CCDevice_t;

void init_device(CCDevice_t *blank, DeviceType device_type, GeneralDeviceType handle);

#endif