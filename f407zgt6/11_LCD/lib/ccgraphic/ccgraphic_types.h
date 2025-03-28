#ifndef CCGRAPHIC_TYPES_H
#define CCGRAPHIC_TYPES_H
#include "ccgraphic_compile_options.h"
#include <stdint.h>
#ifndef device_size_t
typedef unsigned short      device_size_t;
#endif

#ifndef color_t
typedef unsigned short      color_t;
#endif

#ifndef NULL
#define NULL            (void*)0
#endif

typedef struct __basedevice CCDevice_t;

#endif