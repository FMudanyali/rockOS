/**
 *  PIC section of rockOS
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

#include <rockos/pic.h>
#include <stdio.h>

void PIC_remap() {
    printf("Beginning PIC initialization.\n");

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // Start init sequence in cascade mode
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 4); // ICW3: tell Master PIC that theres a slave PIC at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, 0); // ALL THE INTERRUPTS
    outb(PIC2_DATA, 0);
}