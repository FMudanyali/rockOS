/**
 *  Entry point of rockOS
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

#include <stdio.h>
#include <rockos/tty.h>
#include <rockos/keyboard.h>
#include <rockos/pic.h>
#include <rockos/hal.h>
#include <rockos/timer.h>
#include <rockos/paging.h>
#include <rockos/kheap.h>
#include <rockos/multiboot.h>
#include <rockos/rockos.h>
#include <string.h>

extern char _rockos_start, _rockos_end;

static multiboot_info_t* multiboot_info;

void bootloader_init(multiboot_info_t* mbd, unsigned int magic) {
    /* Make sure the magic number matches for memory mapping*/
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("invalid magic number!");
    }
 
    /* Check bit 6 to see if we have a valid memory map */
    if(!(mbd->flags >> 6 & 0x1)) {
        panic("invalid memory map given by GRUB bootloader");
    }
    multiboot_info = mbd;
}

void kernel_main() {   
    printf("Hello, kernel World!\n");
    printf("String test: %s\n", "HELLOOOOO");
    printf("Float test: %.10f\n", 0.123456789);
    printf("Int test: %d\n", 747474);
    printf("Hex test: 0x%x\n", 0xDEADBEEF);
    printf("And now for 0.1 + 0.2...... which is: %.17f\n", 0.1 + 0.2);
    initialize_keyset();
    
    outb(0x70, 0x0); // Seconds
    uint8_t sec = inb(0x71);
    outb(0x70, 0x02); // Minutes
    uint8_t min = inb(0x71);
    outb(0x70, 0x04); // Hours
    uint8_t hour = inb(0x71);
    // BCD to binary conversion
    sec = (sec & 0x0F) + ((sec / 16) * 10);
    min = (min & 0x0F) + ((min / 16) * 10);
    hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
    printf("Time: %02d:%02d:%02d UTC+3\n", (hour + 3) % 24, min, sec);
    
    printf("RockOS Start: %#08X, RockOS End: %#08X\n", &_rockos_start, &_rockos_end);

    for(multiboot_uint32_t i = 0; i < multiboot_info->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t* mmmt = 
            (multiboot_memory_map_t*) (multiboot_info->mmap_addr + i);
 
        printf("Start Addr: %#08X | Length: %#08X | Size: %#08X | Type: %X\n",
            mmmt->addr_low, mmmt->len_low, mmmt->size, mmmt->type);
 
        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
            //
        }
    }

    uint32_t a = kmalloc(8);
    uint32_t b = kmalloc(8);
    uint32_t c = kmalloc(8);
    printf("A: %#08X B: %#08X C: %#08X\n",a,b,c);
    kfree(c);
    kfree(b);
    uint32_t d = kmalloc(8);
    printf("D: %#08X\n", d);

    unsigned char key;
    for(;;) {
        key = readkey();
        if(key)
            putchar(key);
    }

    asm("cli; hlt");
}
