#include <rockos/gdt.h>
#include <stdio.h>
#include <stdint.h>

static uint32_t tss[32];
static gdt_entry_t gdt[GDT_ENTRIES];
static gdtr_t gdtptr;

void gdt_set_descriptor(uint8_t vector, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt[vector].base_low = base & 0xFFFF;
    gdt[vector].base_mid = (base >> 16) & 0xFF;
    gdt[vector].base_high = (base >> 24) & 0xFF;

    gdt[vector].limit_low = limit & 0xFFFF;
    gdt[vector].limit_hi_flags = (limit >> 16) & 0x0F;
    gdt[vector].limit_hi_flags |= flags << 4;

    gdt[vector].access = access;
}

void gdt_init(void) {
    gdt_set_descriptor(0, 0, 0, 0, 0);              // 0x0000 Null Descriptor
    gdt_set_descriptor(1, 0, 0xFFFFF, 0x9A, 0xC);   // 0x0008 Kernel Mode Code Segment
    gdt_set_descriptor(2, 0, 0xFFFFF, 0x92, 0xC);   // 0x0010 Kernel Mode Data Segment
    gdt_set_descriptor(3, 0, 0xFFFFF, 0xFA, 0xC);   // 0x0018 User Mode Code Segment
    gdt_set_descriptor(4, 0, 0xFFFFF, 0xF2, 0xC);   // 0x0020 User Mode Data Segment
    gdt_set_descriptor(5, (uintptr_t)&tss[0], sizeof(tss), 0x89, 0); // 0x0028 Task State Segment

    gdtptr.base = (uintptr_t)&gdt[0];
    gdtptr.limit = sizeof(gdt_entry_t) * GDT_ENTRIES - 1;

    __asm__ volatile("lgdt %0" : : "m"(gdtptr)); // Load new GDT
    __asm__ volatile(
        "jmp $0x0008, $fix_cs\n"
        "fix_cs:\n"
        "mov %cr0, %eax\n"
        "or $1, %al\n" // set PE (Protection Enable) bit in CR0 (Control Register 0)
        "mov %eax, %cr0\n"
        "mov $0x0010, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss"
    );
}
