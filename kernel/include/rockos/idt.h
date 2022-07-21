/**
 *  IDT section header of rockOS
 *  Copyright (C) 2022 Furkan Mudanyali
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, _either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _ROCKOS_IDT_H
#define _ROCKOS_IDT_H

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

static idtr_t idtptr;
static char* err_msg[] = {
    "#DE Divide Error!",
    "#DB Debug Exception!",
    "NMI Interrupt!",
    "#BP Breakpoint!",
    "#OF Overflow!",
    "#BR Bound Range Exceeded!",
    "#Invalid Opcode (Undefined Opcode)!",
    "#NM Device Not Available (No Math Coprocessor)!",
    "#DF Double Fault!",
    "Coprocessor Segment Overrun (reserved)!",
    "#TS Invalid TSS!",
    "#NP Segment Not Present!",
    "#SS Stack-Segment Fault!",
    "#GP General Protection!",
    "#PF Page Fault!",
    "Unknown Reserved Fault 15!",
    "#MF x87 FPU Floating-Point Error (Math Fault)!",
    "#AC Alignment Check!",
    "#MC Machine Check!",
    "#XM SIMD Floating-Point Exception!",
    "#VE Virtualization Exception!",
    "#CP Control Protection Exception!"
};

#endif