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
    cli
    call gdt_init           # initialize the GDT
    movl $stack_top, %esp
    call terminal_initialize
    call idt_init           # initialize the IDT
    call PIC_remap
    sti
    call kernel_main        # transfer control to main kernel
    # Hang if kernel_main unexpectedly returns
1:  hlt
    jmp 1b

.size _start, . - _start
