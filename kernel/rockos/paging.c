#include <rockos/paging.h>
#include <stdint.h>

void load_page_dir(uint32_t* page_dir) {
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

static uint32_t page_dir[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
static uint32_t first_page_table[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

void paging_init() {
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        // Supervisor: only kernel-mode can access
        // Write Enabled: Can be both read and written to
        // Not Present: Page table not present
        page_dir[i] = 0x00000002;
    }
    
    for(unsigned int i = 0; i < PAGE_ENTRIES; i++) {
        // As the address is page aligned, it will leave 12 bits zeroed,
        // which are used by the attributes.
        first_page_table[i] = (i * 0x1000) | 0x103; // attr: supervisor, r/w, present, 9th bit set
    }

    page_dir[0] = ((unsigned int)first_page_table) | 3; //attr: supervisor, r/w, present

    load_page_dir(page_dir);
    enable_paging();
}

uint32_t* get_page() {
    // Search directories for tables
    for (int i = 0; i < PAGE_ENTRIES; ++i) {
        // If table is found (if 2nd and 1st bit are set)
        if((page_dir[i] & 0x3) == 0x3) {
            // Cast table location
            uint32_t *mytable = (uint32_t*)((page_dir[i] >> 12) << 12);
            // Search table for pages
            for(int j = 0; j < PAGE_ENTRIES; ++j) {
                // If page is found (9th, 2nd, 1st bit set)
                if((mytable[j] & 0x103) == 0x103) {
                    mytable[j] &= ~0x100; // Unset 9th bit
                    return &mytable[j];
                }
            } // No page found till last entry
            // Check if next table is not initialized
            if((page_dir[i+1] & 0x3) != 0x3) {
                // The last page of previous table points to the next table
                uint32_t *newtable = &mytable[2*PAGE_ENTRIES];
                for(int k = 0; k < PAGE_ENTRIES; ++k) {
                    newtable[k] = (k * 0x1000) | 0x103; // attr: supervisor, r/w, present, 9th bit set
                }
                page_dir[i+1] = ((unsigned int)newtable) | 3; //attr: supervisor, r/w, present
            }
        }
    }
    return 0;
}

void set_page_free(uint32_t* addr) {
    // Set 9th, 2nd and 1st bits
    *(addr) |= 0x103;
}

uint32_t get_used_memsize() {
    uint32_t used = 0;
    // Iterate over page_dirs
    for(int i = 0; i < PAGE_ENTRIES; ++i) {
        // If a table is available
        if((page_dir[i] & 3) == 3) {
            // Cast the table
            uint32_t* table = ((page_dir[i] >> 12) << 12);
            // Iterate over table
            for(int j = 0; j < PAGE_ENTRIES; ++j) {
                // If a page is unavailable
                if((table[j] & 0x103) != 0x103) {
                    // Then it is used
                    used += 4*4;
                }
            }
        }
    }
    return used;
}