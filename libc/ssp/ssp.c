#include <ssp.h>
#include <stdint.h>
#include <stdlib.h>

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0x74316974
#else
#define STACK_CHK_GUARD 0x7431697474316974
#endif

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn))
void __stack_chk_fail(void){
#if __STDC_HOSTED__
    abort();
#elif __is_rockos_kernel
    panic("Stack smashing detected!");
#endif
}
