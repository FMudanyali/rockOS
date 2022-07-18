#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/paging.h>

#define PAGE_ENTRIES 1024
#define PAGE_SIZE 4 * PAGE_ENTRIES

void paging_init() {
    uint32_t page_dir[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        // Supervisor: only kernel-mode can access
        // Write Enabled: Can be both read and written to
        // Not Present: Page table not present
        page_dir[i] = 0x00000002;
    }

    uint32_t first_page_table[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
    for(unsigned int i = 0; i < PAGE_ENTRIES; i++) {
        // As the address is page aligned, it will leave 12 bits zeroed,
        // which are used by the attributes.
        first_page_table[i] = (i * 0x1000) | 3; // attr: supervisor, r/w, present
    }

    page_dir[0] = ((unsigned int)first_page_table) | 3; //attr: supervisor, r/w, present

    load_page_dir(page_dir);
    enable_paging();
}

void kernel_main(void) {
    paging_init();
    terminal_initialize();
    printf("Hello, kernel World!\n");
    printf("String test: %s\n", "HELLOOOOO");
    printf("Float test: %.10f\n", 0.123456789);
    printf("Int test: %d\n", 747474);
    printf("Hex test: 0x%x\n", 0xDEADBEEF);
    printf("And now for 0.1 + 0.2...... which is: %.17f\n", 0.1 + 0.2);
    asm("int $0");
    asm("cli");
}
