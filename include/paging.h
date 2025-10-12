#pragma once
#include <stdint.h>
#include "page.h"

//structs given in assigment
struct page_directory_entry {
    uint32_t present:1, rw:1, user:1, writethru:1, cachedisabled:1,
             accessed:1, pagesize:1, ignored:2, os_specific:3, frame:20;
};
struct page {
    uint32_t present:1, rw:1, user:1, accessed:1, dirty:1, unused:7, frame:20;
};

void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd);
void loadPageDirectory(struct page_directory_entry *pd);
void enable_paging(void);