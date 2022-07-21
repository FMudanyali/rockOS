#include <stdio.h>

#include <rockos/tty.h>
#include <rockos/keyboard.h>
#include <rockos/pic.h>
#include <rockos/hal.h>
#include <rockos/timer.h>

void kernel_main(void) {
    printf("Hello, kernel World!\n");
    printf("String test: %s\n", "HELLOOOOO");
    printf("Float test: %.10f\n", 0.123456789);
    printf("Int test: %d\n", 747474);
    printf("Hex test: 0x%x\n", 0xDEADBEEF);
    printf("And now for 0.1 + 0.2...... which is: %.17f\n", 0.1 + 0.2);
    initialize_keyset();
    
    outb(0x70, (0x80 << 7) | 0x0); // Seconds
    uint8_t sec = inb(0x71);
    outb(0x70, (0x80 << 7) | 0x02); // Minutes
    uint8_t min = inb(0x71);
    outb(0x70, (0x80 << 7) | 0x04); // Hours
    uint8_t hour = inb(0x71);
    printf("Time: %x:%x:%x\n", hour, min, sec);

    unsigned char oldkey;
    unsigned char key = readkey();
    for(;;){
        oldkey = key;
        key = readkey();
        if (key != '\0') {
            putchar(key);
        }
            
    }

    asm("cli; hlt");
}
