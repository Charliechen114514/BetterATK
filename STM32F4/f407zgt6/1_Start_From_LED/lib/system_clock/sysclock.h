#ifndef SYSCLOCK_H
#define SYSCLOCK_H
#include "precheck.h"

typedef struct __Sysclk_ppl_config{
    uint32_t    n;
    uint32_t    m;
    uint32_t    p;
    uint32_t    q;
}CCCLK_SysConfigType;

void sysclk_init(CCCLK_SysConfigType* type);

/* for commons */
void sysclk_set_as_common168MHz();
uint64_t fetch_sysclk(void);

#endif