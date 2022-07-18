#ifndef _SSP_H
#define _SSP_H

#include <stdint.h>

extern uintptr_t __stack_chk_guard;
void __stack_chk_fail(void);

#endif
