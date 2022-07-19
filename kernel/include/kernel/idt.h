#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>

#define IDT_MAX_DESCRIPTORS 256

typedef struct {
    uint16_t isr_low; // lower 16-bits of ISR address
    uint16_t kernel_cs; // The GDT selector that the CPU will load into CS before calling ISR
    uint8_t reserved; // set to zero
    uint8_t flags; // Type and attributes
    uint16_t isr_high; // Higher 16-bits of ISR address
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

typedef struct {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
} __attribute__((packed)) idt_frame;

typedef struct {
    uint32_t err;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
} __attribute__((packed)) idt_frame_err;

static idtr_t idtptr;

#endif