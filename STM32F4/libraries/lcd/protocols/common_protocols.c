#include "common_protocols.h"

void create_soft_spi_type(SPI_Protocols* protocols,
    Shiftdata shiftdata)
{
    protocols->type = SOFTSPI_TYPE;
    protocols->handle = 0;
    protocols->shiftdata = shiftdata;
}