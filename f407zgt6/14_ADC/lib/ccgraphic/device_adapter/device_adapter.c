#include "device_adapter.h"
#include "device_lcd.h"

void init_device(CCDevice_t *blank, DeviceType device_type, GeneralDeviceType handle)
{
    blank->device_type = device_type;
    switch(blank->device_type)
    {
        case LCD:
            setup_as_lcd(blank, (LCDHandleType*)handle);
        break;
        default:
        break;
    }
}

