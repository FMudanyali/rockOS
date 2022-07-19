/**
 *  Referenced from https://github.com/tendstofortytwo/clay
 *  kernel/kernel.c
 */

#include <kernel/gdt.h>
#include <stdio.h>
#include <stdint.h>

uint32_t TSS[32];
gdt_entry GDT[GDT_ENTRIES];
gdt_ptr gp;

void gdt_fill(gdt_entry* target, gdt* source) {
    if(source->limit > 0xFFFFF) {
        printf("Invalid GDT limit %d\n", source->limit);
        for(;;);
    }

    target->limit_lo = source->limit & 0xFFFF;
    target->limit_hi_flags = (source->limit >> 16) & 0x0F;
    target->base_lo = source->base & 0xFFFF;
    target->base_mid = (source->base >> 16) & 0xFF;
    target->base_hi = (source->base >> 24) & 0xFF;
    target->access = source->access;
    target->limit_hi_flags |= (source->flags << 4);
}

void gdt_init(void) {
    uint32_t tssRef = (uint32_t) TSS;
    gdt rows[GDT_ENTRIES] = {
        {0, 0x00000, 0x00, 0x0}, // 0x0000 Null Descriptor
        {0, 0xFFFFF, 0x9A, 0xC}, // 0x0008 Kernel Mode Code Segment
        {0, 0xFFFFF, 0x92, 0xC}, // 0x0010 Kernel Mode Data Segment
        {0, 0xFFFFF, 0xFA, 0xC}, // 0x0018 User Mode Code Segment
        {0, 0xFFFFF, 0xF2, 0xC}, // 0x0020 User Mode Data Segment
        {tssRef, sizeof(TSS), 0x89, 0x0}, // 0x0028 Task State Segment
    };

    for(uint32_t i = 0; i < GDT_ENTRIES; ++i) {
        gdt_fill(GDT+i, rows+i);
    }

    gp.base = (uintptr_t)&GDT[0];
    gp.limit = sizeof(gdt_entry) * GDT_ENTRIES - 1;

    __asm__ volatile("lgdt %0" : : "m"(gp)); // Load new GDT
    __asm__ volatile(
        "jmp $0x0008, $fix_cs\n"
        "fix_cs:\n"
        "mov $0x0010, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss"
    );
}
