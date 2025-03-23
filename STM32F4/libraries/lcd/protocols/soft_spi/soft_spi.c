#include "soft_spi.h"

void softspi_shiftdata(void* src, spi_shift_data_t data)
{
    LCDHandleType* handle = (LCDHandleType*)src;
    LCDSoftTypeHandlePrivate* soft = (LCDSoftTypeHandlePrivate*)(handle->private_handle);
    CCGPIOType* mosi = &soft->packages_cached->gpios[GPIO_MOSI];
    CCGPIOType* clk = &soft->packages_cached->gpios[GPIO_SCK];
    for (spi_shift_data_t i = 8; i > 0; i--)
    {
        SPI_MOSI_SETAS(mosi->port, mosi->pinType, (data & 0x80));
        SPI_SCLK_CLR(clk->port, clk->pinType);
        SPI_SCLK_SET(clk->port, clk->pinType);
        data <<= 1;
    }
}