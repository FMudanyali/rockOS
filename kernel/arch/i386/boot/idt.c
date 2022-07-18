#include <kernel/idt.h>
#include <stdio.h>

__attribute__((interrupt)) void hello_interrupt(idt_frame* frame) {
    printf("Hello from interrupt!\n");
    printf("ip=0x%x, cs=0x%x, flags=0b%b\n", frame->ip, frame->cs, frame->flags);
}

__attribute__((interrupt)) void hello_interrupt_err(idt_frame_err* frame) {
    printf("Hello from error interrupt!\n");
    printf("ip=0x%x, cs=0x%x, flags=0b%b, err=0x%x\n", frame->ip, frame->cs, frame->flags, frame->err);
    asm volatile("cli; hlt");
}

void idt_fill_row(idt_entry_t* target, idt* source) {
    target->isr_low = source->offset & 0xFFFF;
    target->isr_high = (source->offset >> 16) & 0xFFFF;
    target->kernel_cs = source->segment;
    target->flags = 0x80;
    target->flags |= (source->ring & 0x3) << 5;
    target->flags |= (source->gate & 0xF);
    target->reserved = 0;
}

__attribute__((aligned(0x10))) idt_entry_t IDT[IDT_ROWS];
idtr_t idtptr;

void idt_init(void) {
    idt rows[] = {
        {(uint32_t)hello_interrupt, 0x08, INT_GATE32, 0},
        {(uint32_t)hello_interrupt_err, 0x08, INT_GATE32, 0},
    };

    int valid_rows = sizeof(rows) / sizeof(idt_entry_t);

    for(int i = 0; i < valid_rows; ++i) {
        idt_fill_row(IDT+i, rows+i);
    }

    idt blank_row = {0, 0, 0, 0};
    for(int i = valid_rows; i < IDT_ROWS; ++i) {
        idt_fill_row(IDT+i, &blank_row);
    }

    idtptr.base = &IDT;
    idtptr.limit = (IDT_ROWS * sizeof(idt_entry_t)) - 1;

    idt_load();
}