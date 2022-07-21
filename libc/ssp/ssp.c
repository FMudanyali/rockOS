/**
 *  ssp implementation of rockOS
 *  Copyright (C) 2022 Furkan Mudanyali
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, _either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <ssp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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
    printf("Stack smashing detected!");
    for(;;);
}