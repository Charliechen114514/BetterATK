#ifndef CCLCD_H
#define CCLCD_H
#include "lcd_types.h"
#include "led/gpio.h"
#include "lcd/lcd_color_type.h"
#include "protocols/common_protocols.h"

typedef struct __lcd_handle LCDHandleType;
typedef struct __lcd_low_level_operations{
    void(*reset)(LCDHandleType*);
    void(*switch_directions)(LCDHandleType*, LCD_Directions);
    void(*light_up_backlight)(LCDHandleType*, LCD_BackLightStatus);
    void(*set_mapping_window)(LCDHandleType*, LCDRawPoint begin, LCDRawPoint end);
    void(*clear_lcd)(LCDHandleType*, LCD_Color color);
    void(*filled_lcd)(LCDHandleType*, LCDRawPoint begin, LCDRawPoint end, LCD_Color color);
    void(*drawPoint)(LCDHandleType*, LCDRawPoint point, LCD_Color color);
    void(*set_cursor)(LCDHandleType*, LCDRawPoint point);
    void(*send_data)(LCDHandleType*, uint16_t data);
    void(*drawArea)(LCDHandleType*, LCDRawPoint point, lcd_size_t width, lcd_size_t height, uint8_t* src);
    lcd_command_type    write_ram_cmd_cached;
    lcd_command_type    set_x_command;
    lcd_command_type    set_y_command;
}LCDLowlevelOperations;

typedef void*       RawLCDType;
typedef struct __lcd_handle {
    RawLCDType              private_handle;
    LCD_BaseProperty        lcd_property;
    LCDLowlevelOperations   operations;
    SPI_Protocols           protocols;
}LCDHandleType;

typedef struct __softpack{
    CCGPIOType          gpios[SOFT_GPIO_INIT_ARRAY_N];
    void(*gpio_clk_open)();
}SoftLCDInitGPIOPack;

typedef struct {
    SoftLCDInitGPIOPack*    packages_cached;
}LCDSoftTypeHandlePrivate;

void init_lcdhandle_soft(
    LCDHandleType* initer, 
    LCD_DeviceType devicet_type,
    LCDSoftTypeHandlePrivate* packages);


#endif