#include "st7796/lcdst7796_property.h"
#include "st7796/lcdst7796_reg.h"
#include "tools/lcd_math.h"
#include "lcd_types.h"
#include "system_clock.h"

#include "lcd_commons/lcd_soft_common.h"
#include "protocols/soft_spi/soft_spi.h"

void on_init_st7796property(LCD_BaseProperty *property)
{
    property->lcd_height = ST7796_HEIGHT;
    property->lcd_width = ST7796_WIDTH;
    property->id = 0; // id_unused
}

static void resetlcd(LCDHandleType *handle)
{
    LCDSoftTypeHandlePrivate *soft = (LCDSoftTypeHandlePrivate *)(handle->private_handle);
    CCGPIOType *rst = &soft->packages_cached->gpios[GPIO_RST];
    LCD_RST_CLR(rst->port, rst->pinType);
    system_delay_ms(100);
    LCD_RST_SET(rst->port, rst->pinType);
    system_delay_ms(50);
}

static void send_cmd(LCDHandleType *handle, uint8_t command)
{
    LCDSoftTypeHandlePrivate *soft = (LCDSoftTypeHandlePrivate *)(handle->private_handle);
    CCGPIOType *cs = &soft->packages_cached->gpios[GPIO_CS];
    CCGPIOType *rs = &soft->packages_cached->gpios[GPIO_DCRS];
    LCD_CS_CLR(cs->port, cs->pinType);
    LCD_RS_CLR(rs->port, rs->pinType);
    handle->protocols.shiftdata(handle, command);
    LCD_CS_SET(cs->port, cs->pinType);
}

static void send_data(LCDHandleType *handle, uint8_t data)
{
    LCDSoftTypeHandlePrivate *soft = (LCDSoftTypeHandlePrivate *)(handle->private_handle);
    CCGPIOType *cs = &soft->packages_cached->gpios[GPIO_CS];
    CCGPIOType *rs = &soft->packages_cached->gpios[GPIO_DCRS];
    LCD_CS_CLR(cs->port, cs->pinType);
    LCD_RS_SET(rs->port, rs->pinType);
    handle->protocols.shiftdata(handle, data);
    LCD_CS_SET(cs->port, cs->pinType);
}

void send_worddata(LCDHandleType *handle, uint16_t data)
{
    LCDSoftTypeHandlePrivate *soft = (LCDSoftTypeHandlePrivate *)(handle->private_handle);
    CCGPIOType *cs = &soft->packages_cached->gpios[GPIO_CS];
    CCGPIOType *rs = &soft->packages_cached->gpios[GPIO_DCRS];
    LCD_CS_CLR(cs->port, cs->pinType);
    LCD_RS_SET(rs->port, rs->pinType);
    handle->protocols.shiftdata(handle, data >> 8);
    handle->protocols.shiftdata(handle, data);
    LCD_CS_SET(cs->port, cs->pinType);
}

static void soft_target_register(LCDHandleType *handle, uint8_t reg, uint8_t data)
{
    send_cmd(handle, reg);
    send_data(handle, data);
}

static void switch_positions(LCDHandleType *handle, LCD_Directions directions)
{
    handle->operations.write_ram_cmd_cached = 0x2c;
    handle->operations.set_x_command = 0x2a;
    handle->operations.set_y_command = 0x2b;
    lcd_size_t new_width = ST7796_WIDTH;
    lcd_size_t new_height = ST7796_HEIGHT;
    uint16_t value = 0;
    switch (directions)
    {
    case LCD_Direction_0:
        value = (1 << 3) | (1 << 6);
        break;
    case LCD_Direction_90:
        new_width = ST7796_HEIGHT;
        new_height = ST7796_WIDTH;
        value = (1 << 3) | (1 << 5);
        break;
    case LCD_Direction_180:
        value = (1 << 3) | (1 << 7);
        break;
    case LCD_Direction_270:
        new_width = ST7796_HEIGHT;
        new_height = ST7796_WIDTH;
        value = (1 << 3) | (1 << 7) | (1 << 6) | (1 << 5);
        break;
    default:
        value = (1 << 3) | (1 << 6);
        break;
    }
    handle->lcd_property.lcd_height = new_height;
    handle->lcd_property.lcd_width = new_width;
    soft_target_register(handle, ST7796_DIRECTIONS_REGISTER, value);
}

static void light_up_backlights(LCDHandleType *handle, LCD_BackLightStatus should_lightup)
{
    LCDSoftTypeHandlePrivate *soft = (LCDSoftTypeHandlePrivate *)(handle->private_handle);
    CCGPIOType *led = &soft->packages_cached->gpios[GPIO_LED];
    LCD_LED_SET_AS(led->port, led->pinType, should_lightup);
}

static void set_mappings_window(LCDHandleType *handle, LCDRawPoint begin, LCDRawPoint end)
{
    send_cmd(handle, handle->operations.set_x_command);
    send_data(handle, begin.x >> 8);
    send_data(handle, begin.x & 0x00FF);
    send_data(handle, end.x >> 8);
    send_data(handle, end.x & 0x00FF);
    send_cmd(handle, handle->operations.set_y_command);
    send_data(handle, begin.y >> 8);
    send_data(handle, begin.y & 0x00FF);
    send_data(handle, end.y >> 8);
    send_data(handle, end.y & 0x00FF);
    send_cmd(handle, handle->operations.write_ram_cmd_cached);
}

static void clear_lcd(LCDHandleType *handle, LCD_Color color)
{
    lcd_size_t height = handle->lcd_property.lcd_height;
    lcd_size_t width = handle->lcd_property.lcd_width;
    LCDRawPoint begin = {
        .x = 0,
        .y = 0};

    LCDRawPoint end = {
        .x = width - 1,
        .y = height - 1};

    set_mappings_window(handle, begin, end);
    for (lcd_size_t i = 0; i < height; i++)
    {
        for (lcd_size_t j = 0; j < width; j++)
        {
            send_worddata(handle, color);
        }
    }
}

static void filled_lcd(
    LCDHandleType *handle,
    LCDRawPoint begin, LCDRawPoint end,
    LCD_Color color)
{
    set_mappings_window(handle, begin, end);
    lcd_size_t width = fetch_positive_delta(begin.x, end.x) + 1;
    lcd_size_t height = fetch_positive_delta(begin.y, end.y) + 1;
    for (lcd_size_t i = 0; i < height; i++)
    {
        for (lcd_size_t j = 0; j < width; j++)
        {
            send_worddata(handle, color);
        }
    }
    LCDRawPoint _begin = {
        .x = 0,
        .y = 0};

    LCDRawPoint _end = {
        .x = width - 1,
        .y = height - 1};
    set_mappings_window(handle, _begin, _end);
}

static void set_cursor(LCDHandleType *handle, LCDRawPoint point)
{
    set_mappings_window(handle, point, point);
}

static void draw_point(LCDHandleType *handle, LCDRawPoint point, LCD_Color color)
{
    set_cursor(handle, point);
    send_worddata(handle, color);
}

static void draw_area(LCDHandleType *handle, LCDRawPoint point, lcd_size_t width, lcd_size_t height, uint8_t *src)
{
    LCDRawPoint br = {
        .x = point.x + width - 1,
        .y = point.y + height - 1};
    unsigned char low_byte, high_byte;

    set_mappings_window(handle, point, br);
    for (uint64_t i = 0; i < width * height; i++)
    {
        low_byte = *(src + i * 2); // 数据低位在前
        high_byte = *(src + i * 2 + 1);
        send_worddata(handle, high_byte << 8 | low_byte);
    }

    LCDRawPoint _begin = {
        .x = 0,
        .y = 0};

    LCDRawPoint _end = {
        .x = width - 1,
        .y = height - 1};
    set_mappings_window(handle, _begin, _end);
}

void on_filled_st7796commands(LCDHandleType *handle)
{
    handle->operations.reset = resetlcd;
    handle->operations.switch_directions = switch_positions;
    handle->operations.light_up_backlight = light_up_backlights;
    handle->operations.set_mapping_window = set_mappings_window;
    handle->operations.clear_lcd = clear_lcd;
    handle->operations.filled_lcd = filled_lcd;
    handle->operations.drawPoint = draw_point;
    handle->operations.set_cursor = set_cursor;
    handle->operations.send_data = send_worddata;
    handle->operations.drawArea = draw_area;
    handle->operations.write_ram_cmd_cached = 0x2c;
    handle->operations.set_x_command = 0x2a;
    handle->operations.set_y_command = 0x2b;
    create_soft_spi_type(&handle->protocols, (Shiftdata)softspi_shiftdata);
}

typedef struct
{
    uint8_t cmd;
    const uint8_t *data;
    uint8_t datalen;
} LCD_InitCmd;

const uint8_t data_f0_1[] = {0xC3};
const uint8_t data_f0_2[] = {0x96};
const uint8_t data_36[] = {0x68};
const uint8_t data_3a[] = {0x05};
const uint8_t data_b0[] = {0x80};
const uint8_t data_b6[] = {0x00, 0x02};
const uint8_t data_b5[] = {0x02, 0x03, 0x00, 0x04};
const uint8_t data_b1[] = {0x80, 0x10};
const uint8_t data_b4[] = {0x00};
const uint8_t data_b7[] = {0xC6};
const uint8_t data_c5[] = {0x24};
const uint8_t data_e4[] = {0x31};
const uint8_t data_e8[] = {0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33};
const uint8_t data_e0[] = {0xF0, 0x09, 0x13, 0x12, 0x12, 0x2B, 0x3C, 0x44, 0x4B, 0x1B, 0x18, 0x17, 0x1D, 0x21};
const uint8_t data_e1[] = {0xF0, 0x09, 0x13, 0x0C, 0x0D, 0x27, 0x3B, 0x44, 0x4D, 0x0B, 0x17, 0x17, 0x1D, 0x21};
const uint8_t data_36_2[] = {0xEC};
const uint8_t data_f0_3[] = {0xC3};
const uint8_t data_f0_4[] = {0x69};

const LCD_InitCmd lcd_init_cmds[] = {
    {0xF0, data_f0_1, sizeof(data_f0_1)},
    {0xF0, data_f0_2, sizeof(data_f0_2)},
    {0x36, data_36, sizeof(data_36)},
    {0x3A, data_3a, sizeof(data_3a)},
    {0xB0, data_b0, sizeof(data_b0)},
    {0xB6, data_b6, sizeof(data_b6)},
    {0xB5, data_b5, sizeof(data_b5)},
    {0xB1, data_b1, sizeof(data_b1)},
    {0xB4, data_b4, sizeof(data_b4)},
    {0xB7, data_b7, sizeof(data_b7)},
    {0xC5, data_c5, sizeof(data_c5)},
    {0xE4, data_e4, sizeof(data_e4)},
    {0xE8, data_e8, sizeof(data_e8)},
    {0xC2, NULL, 0},
    {0xA7, NULL, 0},
    {0xE0, data_e0, sizeof(data_e0)},
    {0xE1, data_e1, sizeof(data_e1)},
    {0x36, data_36_2, sizeof(data_36_2)},
    {0xF0, data_f0_3, sizeof(data_f0_3)},
    {0xF0, data_f0_4, sizeof(data_f0_4)},
    {0x13, NULL, 0},
    {0x11, NULL, 0},
    {0x29, NULL, 0},
};

void on_inter_setup_st7796soft(LCDHandleType *handle)
{
    handle->operations.reset(handle);
    for (int i = 0; i < sizeof(lcd_init_cmds) / sizeof(LCD_InitCmd); i++)
    {
        send_cmd(handle, lcd_init_cmds[i].cmd);
        for (int j = 0; j < lcd_init_cmds[i].datalen; j++)
        {
            send_data(handle, lcd_init_cmds[i].data[j]);
        }
    }
}
