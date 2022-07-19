#include <rockos/paging.h>
#include <stdint.h>

void load_page_dir(uint32_t page_dir) {
    asm volatile (
        "mov %%eax, %%cr3"
        :
        : "a"(page_dir)
        : "memory"
    );
}

void enable_paging() {
    asm volatile (
        "mov %cr0, %eax\n"
        "or $0x80000000, %eax\n"
        "mov %eax, %cr0"
    );
}

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