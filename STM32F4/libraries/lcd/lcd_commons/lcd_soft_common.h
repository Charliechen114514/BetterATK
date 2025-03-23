#ifndef LCD_SOFT_COMMON_H
#define LCD_SOFT_COMMON_H
#include "lcd_compile_config.h"

/* soft operations */
#define SET_AS_HIGH(port, pin)                      \
    do                                              \
    {                                               \
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); \
    } while (0)
#define SET_AS_LOW(port, pin)                         \
    do                                                \
    {                                                 \
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); \
    } while (0)

#define SET_AS(port, pin, state)                 \
    do                                           \
    {                                            \
        HAL_GPIO_WritePin(port, pin, !!(state)); \
    } while (0)

#ifndef LCD_CS_SET
#define LCD_CS_SET(port, pin) SET_AS_HIGH(port, pin)
#endif

#ifndef LCD_RS_SET
#define LCD_RS_SET(port, pin) SET_AS_HIGH(port, pin)
#endif

#ifndef LCD_RST_SET
#define LCD_RST_SET(port, pin) SET_AS_HIGH(port, pin)
#endif

#ifndef LCD_CS_CLR
#define LCD_CS_CLR(port, pin) SET_AS_LOW(port, pin)
#endif

#ifndef LCD_RS_CLR
#define LCD_RS_CLR(port, pin) SET_AS_LOW(port, pin)
#endif

#ifndef LCD_RST_CLR
#define LCD_RST_CLR(port, pin) SET_AS_LOW(port, pin)
#endif
#ifndef LCD_LED_SET_AS
#define LCD_LED_SET_AS(port, pin, state) SET_AS(port, pin, state)
#endif
#endif