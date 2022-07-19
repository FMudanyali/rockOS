/**
 * Referenced from https://github.com/tendstofortytwo/clay
 * kernel/kernel.c
 */

#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stdint.h>

#define GDT_ENTRIES 0x6

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr;

typedef struct {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_hi_flags;
    uint8_t base_hi;
} __attribute__((packed)) gdt_entry;

typedef struct {
    uint32_t base;
    uint32_t limit;
    uint8_t access;
    uint8_t flags;
} __attribute__((packed)) gdt;

void gdt_fill(gdt_entry*, gdt*);
void gdt_init(void);

extern uint32_t TSS[32];
extern gdt_entry GDT[GDT_ENTRIES];
extern gdt_ptr gp;

#endif
