#ifndef SOFT_SPI_H
#define SOFT_SPI_H
#include "lcd.h"
#include "lcd_commons/lcd_soft_common.h"
#ifndef SPI_MOSI_SET
#define SPI_MOSI_SET(port, pin)     SET_AS_HIGH(port, pin)
#endif

#ifndef SPI_MOSI_CLR
#define SPI_MOSI_CLR(port, pin)     SET_AS_LOW(port, pin)
#endif

#ifndef SPI_MOSI_SETAS
#define SPI_MOSI_SETAS(port, pin, state) SET_AS(port, pin, state)
#endif

#ifndef SPI_SCLK_SET
#define SPI_SCLK_SET(port, pin)     SET_AS_HIGH(port, pin)
#endif

#ifndef SPI_SCLK_CLR
#define SPI_SCLK_CLR(port, pin)     SET_AS_LOW(port, pin)
#endif

void  softspi_shiftdata(void* handle, spi_shift_data_t data);

#endif