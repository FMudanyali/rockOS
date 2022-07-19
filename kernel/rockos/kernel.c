#include <stdio.h>

#include <rockos/tty.h>
#include <rockos/gdt.h>

void kernel_main(void) {
    printf("Hello, kernel World!\n");
    printf("String test: %s\n", "HELLOOOOO");
    printf("Float test: %.10f\n", 0.123456789);
    printf("Int test: %d\n", 747474);
    printf("Hex test: 0x%x\n", 0xDEADBEEF);
    printf("And now for 0.1 + 0.2...... which is: %.17f\n", 0.1 + 0.2);

    unsigned char oldkey;
    unsigned char key = readkey();
    for(;;){
        oldkey = key;
        key = readkey();
        if (key != oldkey)
            putchar(key);
    }

    asm("cli; hlt");
}
