#include <stdio.h>

__attribute__((noreturn))
void exception_handler(void);
void exception_handler() {
    printf("Kernel exception!\n");
    __asm__ volatile ("cli; hlt");
}