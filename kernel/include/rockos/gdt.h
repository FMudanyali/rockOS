#ifndef _ROCKOS_GDT_H
#define _ROCKOS_GDT_H

#include <stdint.h>

#define GDT_ENTRIES 0x6

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdtr_t;

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_hi_flags;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

#endif