#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>

#define IDT_ROWS 256
#define TASK_GATE 0x5
#define INT_GATE16 0x6
#define TRAP_GATE16 0x7
#define INT_GATE32 0xE
#define TRAP_GATE32 0xF

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

typedef struct {
    uint16_t isr_low; // lower 16-bits of ISR address
    uint16_t kernel_cs; // The GDT selector that the CPU will load into CS before calling ISR
    uint8_t reserved; // set to zero
    uint8_t flags; // Type and attributes
    uint16_t isr_high; // Higher 16-bits of ISR address
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint32_t offset;
    uint16_t segment;
    uint8_t gate;
    uint8_t ring;
} idt;

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

extern void idt_load();
extern idtr_t idtptr;

#endif