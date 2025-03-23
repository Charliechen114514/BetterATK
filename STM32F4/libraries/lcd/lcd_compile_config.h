#ifndef LCD_COMPILE_CONFIG_H
#define LCD_COMPILE_CONFIG_H
#define ACCEPT_ST7796

/* lcd supportive types */
typedef enum {
    /*   Supportive Classes   */
#ifdef ACCEPT_ST7796
    LCD_ST7796,
#endif
    LCD_Dummy
}LCD_DeviceType;

typedef unsigned short  lcd_size_t;
typedef unsigned char   lcd_id_t;
typedef unsigned char   lcd_direction_t;
typedef unsigned short  lcd_command_type;

typedef unsigned char   spi_shift_data_t;

/* defines the array offsets of the gpios */


#define GPIO_CS (0)
#define GPIO_RST (1)
#define GPIO_DCRS (2)
#define GPIO_MOSI (3)
#define GPIO_SCK (4)
#define GPIO_LED (5)
#define GPIO_MISO (6)
#define SOFT_GPIO_INIT_ARRAY_N (7)

#endif