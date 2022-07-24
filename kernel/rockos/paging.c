#include <rockos/paging.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

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

extern char _rockos_end;
static uint32_t* page_dir __attribute__((packed));

void paging_init() {
    page_dir = (uint32_t*)(((uint32_t)&_rockos_end & ~0xFFF) + 0x1000);
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        // Supervisor: only kernel-mode can access
        // Write Enabled: Can be both read and written to
        // Not Present: Page table not present
        *(page_dir + i) = 0x00000002;
    }

    uint32_t* firstPageTable __attribute__((packed)) = (uint32_t*)(((uint32_t)page_dir & ~0xFFF) + 0x1000);
    for(unsigned int i = 0; i < PAGE_ENTRIES; i++) {
        // As the address is page aligned, it will leave 12 bits zeroed,
        // which are used by the attributes.
        firstPageTable[i] = (i*0x1000) | 0x003; // attr: supervisor, r/w, present, 9th bit set
    }

    page_dir[0] = (uint32_t)firstPageTable | 0x003; //attr: supervisor, r/w, present
    load_page_dir(page_dir);
    enable_paging();
}

void alloc_pages(uint32_t* pages, size_t size) {
    for(size_t i = 0; i < size;) {
        for(size_t j = 0; j < PAGE_ENTRIES && i < size; ++j) {
            printf("oklahoma %#08X\n", page_dir[j]);
            if((page_dir[j] & 3) == 3) {
                uint32_t* pageTable __attribute__((packed)) = page_dir[j] & ~0xFFF;
                for(int k = 0; k < PAGE_ENTRIES && i < size; ++k) {
                    if((pageTable[k] & 3) == 3 && (pageTable[k] & 0x100) != 0x100) {
                        pageTable[k] |= 0x100;
                        pages[i] = ((uint32_t)pageTable & ~0xFFF) | (pageTable[k] >> 12);
                        printf("connecticut %#08X\n", pages[i]);
                        ++i;
                    }
                }
            } else {
                printf("aa %#08X\n", (page_dir[j-1] & ~0xFFF) + 0x4000);
                uint32_t* newPageTable __attribute__((packed)) = (page_dir[j-1] & ~0xFFF) + 0x4000;
                printf("alabama %#08X, %#08X\n", newPageTable[0], (page_dir[j-1] & ~0xFFF) + 0x4000);
                for(unsigned int l = 0; l < PAGE_ENTRIES; l++) {
                    // As the address is page aligned, it will leave 12 bits zeroed,
                    // which are used by the attributes.
                    newPageTable[l] = (l*0x1000) | 0x003; // attr: supervisor, r/w, present, 9th bit set
                }
                page_dir[j] = (uint32_t)newPageTable | 0x003;
                printf("NewPageTable: %#08X\n", page_dir[j]);
                --j;
            }
        }
    }
}