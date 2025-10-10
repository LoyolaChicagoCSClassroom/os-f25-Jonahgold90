#pragma once
#include <stdint.h>
#include <stddef.h>

//define struct pPage
struct ppage {
    struct ppage *next;
    struct ppage *prev;
    void *physical_addr;
};

//initialize free list
void init_pfa_list(void);

struct ppage* allocate_physical_pages(unsigned int num_pages);

void free_physical_pages(struct ppage* page_list);