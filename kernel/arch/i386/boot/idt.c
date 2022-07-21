#include <rockos/idt.h>
#include <rockos/keyboard.h>
#include <rockos/timer.h>
#include <stdio.h>

__attribute__((interrupt)) void int_handler(void* frame, unsigned int number) {
    if (number <= 21) {
        printf("%s\n", err_msg[number]);
    }
    asm("cli;hlt");
}

#define INT_WRAPPER(NUM) __attribute__((interrupt)) void int##NUM##_wrapper(void* frame) { \
    int_handler(frame, NUM); \
}

INT_WRAPPER(0)
INT_WRAPPER(1)
INT_WRAPPER(2)
INT_WRAPPER(3)
INT_WRAPPER(4)
INT_WRAPPER(5)
INT_WRAPPER(6)
INT_WRAPPER(7)
INT_WRAPPER(8)
INT_WRAPPER(9)
INT_WRAPPER(10) // Invalid TSS (IRQ2)
INT_WRAPPER(11) // Segment not Present (IRQ3)
INT_WRAPPER(12) // Stack Fault (IRQ4)
INT_WRAPPER(13) // GP Fault (IRQ5)
INT_WRAPPER(14) // Page Fault (IRQ6)
INT_WRAPPER(15) // Reserved (IRQ7)
INT_WRAPPER(16)
INT_WRAPPER(17)
INT_WRAPPER(18)
INT_WRAPPER(19)
INT_WRAPPER(20)
INT_WRAPPER(21)
// Reserved from now on
INT_WRAPPER(22)
INT_WRAPPER(23)
INT_WRAPPER(24)
INT_WRAPPER(25)
INT_WRAPPER(26)
INT_WRAPPER(27)
INT_WRAPPER(28)
INT_WRAPPER(29)
INT_WRAPPER(30)
INT_WRAPPER(31)

static __attribute__((aligned(0x10))) idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idtr_t idtptr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];
    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08; // Whatever offset kernel code selector is in GDT
    descriptor->flags = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

void idt_init(void) {
    idtptr.base = (uintptr_t)&idt[0];
    idtptr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    idt_set_descriptor(0, int0_wrapper, 0x8E);
    idt_set_descriptor(1, int1_wrapper, 0x8E);
    idt_set_descriptor(2, int2_wrapper, 0x8E);
    idt_set_descriptor(3, int3_wrapper, 0x8E);
    idt_set_descriptor(4, int4_wrapper, 0x8E);
    idt_set_descriptor(5, int5_wrapper, 0x8E);
    idt_set_descriptor(6, int6_wrapper, 0x8E);
    idt_set_descriptor(7, int7_wrapper, 0x8E);
    idt_set_descriptor(8, int8_wrapper, 0x8E);
    idt_set_descriptor(9, int9_wrapper, 0x8E);
    idt_set_descriptor(10, int10_wrapper, 0x8E);
    idt_set_descriptor(11, int11_wrapper, 0x8E);
    idt_set_descriptor(12, int12_wrapper, 0x8E);
    idt_set_descriptor(13, int13_wrapper, 0x8E);
    idt_set_descriptor(14, int14_wrapper, 0x8E);
    idt_set_descriptor(15, int15_wrapper, 0x8E); // Reserved
    idt_set_descriptor(16, int16_wrapper, 0x8E);
    idt_set_descriptor(17, int17_wrapper, 0x8E);
    idt_set_descriptor(18, int18_wrapper, 0x8E);
    idt_set_descriptor(19, int19_wrapper, 0x8E);
    idt_set_descriptor(20, int20_wrapper, 0x8E);
    idt_set_descriptor(21, int21_wrapper, 0x8E);
    // Reserved
    idt_set_descriptor(22, int22_wrapper, 0x8E);
    idt_set_descriptor(23, int23_wrapper, 0x8E);
    idt_set_descriptor(24, int24_wrapper, 0x8E);
    idt_set_descriptor(25, int25_wrapper, 0x8E);
    idt_set_descriptor(26, int26_wrapper, 0x8E);
    idt_set_descriptor(27, int27_wrapper, 0x8E);
    idt_set_descriptor(28, int28_wrapper, 0x8E);
    idt_set_descriptor(29, int29_wrapper, 0x8E);
    idt_set_descriptor(30, int30_wrapper, 0x8E);
    idt_set_descriptor(31, int31_wrapper, 0x8E);
    // IRQs
    idt_set_descriptor(32, timer_handler, 0x8E);
    timer_phase(100);
    idt_set_descriptor(33, keyboard_handler, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtptr)); // Load new IDT
}