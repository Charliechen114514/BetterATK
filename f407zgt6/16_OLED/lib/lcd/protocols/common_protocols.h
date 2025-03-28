#ifndef COMMON_PROTOCOL_H
#define COMMON_PROTOCOL_H

typedef enum {
    SOFTSPI_TYPE,
    HARDSPI_TYPE
}SPIType;

typedef struct __spi_protocols {
    SPIType type;
    void*   handle;
    void(*shiftdata)(void* src, unsigned short data);
}SPI_Protocols;

typedef void(*Shiftdata)(void* src, unsigned short data);

void create_soft_spi_type(
    SPI_Protocols* protocols, 
    Shiftdata   shiftdata);

#endif