# Referenced from https://github.com/tendstofortytwo/clay
# kernel/boot/gdt.s

.global gdt_load
.type gdt_load, @function
gdt_load:
    lgdt (gp)
    mov %cr0, %eax
    or $0x1, %eax
    mov %eax, %cr0
    jmp $0x08, $reload

reload:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    ret
