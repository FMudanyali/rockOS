#include <kernel/idt.h>
#include <stdio.h>

__attribute__((interrupt)) void int_handler(void* frame, unsigned int number) {
    unsigned int err = 0;
    idt_frame* idt_ptr;
    idt_frame_err* idt_ptr_e;
    switch(number) {
        case 8:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 17:
        case 21:
            idt_ptr_e = (idt_frame_err*)frame;
            err = 1;
            break;
        default:
            idt_ptr = (idt_frame*)frame;
            break;
    }

    switch(number) {
        case 0:
            printf("#DE Divide Error!\n");
            break;
        case 1:
            printf("#DB Debug Exception!\n");
            break;
        case 2:
            printf("NMI Interrupt!\n");
            break;
        case 3:
            printf("#BP Breakpoint!\n");
            break;
        case 4:
            printf("#OF Overflow!\n");
            break;
        case 5:
            printf("#BR Bound Range Exceeded!\n");
            break;
        case 6:
            printf("#Invalid Opcode (Undefined Opcode)!\n");
            break;
        case 7:
            printf("#NM Device Not Available (No Math Coprocessor)!\n");
            break;
        case 9:
            printf("Coprocessor Segment Overrun (reserved)!\n");
            break;
        case 16:
            printf("#MF x87 FPU Floating-Point Error (Math Fault)!\n");
            break;
        case 18:
            printf("#MC Machine Check!\n");
            break;
        case 19:
            printf("#XM SIMD Floating-Point Exception!\n");
            break;
        case 20:
            printf("#VE Virtualization Exception!\n");
            break;
        ////////
        case 8:
            printf("#DF Double Fault!\n");
            break;
        case 10:
            printf("#TS Invalid TSS!\n");
            break;
        case 11:
            printf("#NP Segment Not Present!\n");
            break;
        case 12:
            printf("#SS Stack-Segment Fault!\n");
            break;
        case 13:
            printf("#GP General Protection!\n");
            break;
        case 14:
            printf("#PF Page Fault!\n");
            break;
        case 17:
            printf("#AC Alignment Check!\n");
            break;
        case 21:
            printf("#CP Control Protection Exception!\n");
            break;
    }

    if(err){
    printf("err: 0x%X, ip: 0x%X, cs: 0x%X, flags: 0x%X, sp: 0x%X, ss: 0x%X\n",
        idt_ptr_e->err, idt_ptr_e->ip, idt_ptr_e->cs, idt_ptr_e->flags, idt_ptr_e->sp, idt_ptr_e->ss);
    } else {
    printf("ip: 0x%X, cs: 0x%X, flags: 0x%X, sp: 0x%X, ss: 0x%X\n",
        idt_ptr->ip, idt_ptr->cs, idt_ptr->flags, idt_ptr->sp, idt_ptr->ss);
    }
    return;
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
INT_WRAPPER(10)
INT_WRAPPER(11)
INT_WRAPPER(12)
INT_WRAPPER(13)
INT_WRAPPER(14)
INT_WRAPPER(16)
INT_WRAPPER(17)
INT_WRAPPER(18)
INT_WRAPPER(19)
INT_WRAPPER(20)
INT_WRAPPER(21)

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
    idt_set_descriptor(16, int16_wrapper, 0x8E);
    idt_set_descriptor(17, int17_wrapper, 0x8E);
    idt_set_descriptor(18, int18_wrapper, 0x8E);
    idt_set_descriptor(19, int19_wrapper, 0x8E);
    idt_set_descriptor(20, int20_wrapper, 0x8E);
    idt_set_descriptor(21, int21_wrapper, 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtptr)); // Load new IDT
    __asm__ volatile("sti"); // Set interrupt flag
}