#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

extern void load_page_dir(uint32_t*);
extern void enable_paging();

#endif