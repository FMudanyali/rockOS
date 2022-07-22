#include <stdio.h>

#include <rockos/tty.h>
#include <rockos/keyboard.h>
#include <rockos/pic.h>
#include <rockos/hal.h>
#include <rockos/timer.h>
#include <rockos/paging.h>

void kernel_main(void) {
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

    uint32_t first = get_used_memsize();
    uint32_t limit = 1024 * 8;
    uint32_t* pages[limit];

    for(uint32_t i = 0; i < limit; i++) {
        pages[i] = get_page();
    }
    uint32_t middle = get_used_memsize();

    for(uint32_t i = 0; i < limit; i++) {
        set_page_free(pages[i]);
    }
    uint32_t last = get_used_memsize();
    
    printf("Before Allocation: %d KiB\nAfter allocation: %d KiB\nAfter Clear: %d KiB\n", first, middle, last);
    printf("Finished!\n");

    unsigned char key;
    for(;;) {
        key = readkey();
        if(key)
            putchar(key);
    }

    asm("cli; hlt");
}
