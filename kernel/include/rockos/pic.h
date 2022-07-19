#ifndef _ROCKOS_PIC_H
#define _ROCKOS_PIC_H

#include <stdint.h>
#include <rockos/hal.h>

#define PIC1            0x20
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC1_OFFSET     0x20

#define PIC2            0xA0
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1
#define PIC2_OFFSET     0x28

#define PIC_EOI         0x20

#define ICW1_ICW4       0x01 // ICW4 (not) needed
#define ICW1_SINGLE     0X02 // Single (cascade) mode
#define ICW1_INTERVAL4  0x04 // Call address interval 4 (8)
#define ICW1_LEVEL      0x08 // Level triggered (edge) mode
#define ICW1_INIT       0x10 // Initialization - required

#define ICW4_8086       0x01 // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO       0x02 // Auto (normal) EDI
#define ICW4_BUF_SLAVE  0x08 // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C // Buffered mode/master
#define ICW4_SFNM       0x10 // Special fully nested (not)

static inline void io_wait() {
    outb(0x80, 0);
}

static inline void pic_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

#endif