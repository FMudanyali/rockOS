/**
 *  Boot section of rockOS
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

# Declare constants for multiboot header
.set ALIGN,     1<<0                # align loaded modules on page boundaries
.set MEMINFO,   1<<1                # provide memory map
.set FLAGS,     ALIGN | MEMINFO     # multiboot flag field
.set MAGIC,     0x1BADB002          # magic number for bootloader to find header
.set CHECKSUM,  -(MAGIC + FLAGS)    # checksum of above

# Declare header in multiboot standard
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for initial thread
.section .bss
.align 16
stack_bottom:
.skip 32768 # 32 KiB
stack_top:

# kernel entry point
.section .text
.global _start
.type _start, @function
_start:
    push %eax
    push %ebx
    call bootloader_init
    cli
    call gdt_init           # initialize the GDT
    movl $stack_top, %esp
    call terminal_initialize
    call idt_init           # initialize the IDT
    call PIC_remap
    sti
    call paging_init
    call kernel_main        # transfer control to main kernel
    # Hang if kernel_main unexpectedly returns
1:  hlt
    jmp 1b

.size _start, . - _start
