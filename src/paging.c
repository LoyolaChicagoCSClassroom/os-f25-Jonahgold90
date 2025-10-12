#include "paging.h"
#include "page.h"
#include <stdint.h>
#include <stddef.h>

static void *k_memset(void *p, int v, size_t n) {
    //pointer to the very start of the memory
    unsigned char *ptr = p;
    while (n--) {
        //write value v
        *ptr++ = (unsigned char)v;
    }
    return p;
}

#define memset k_memset

#define PAGE_SIZE 4096
//num of second level page tables
#define MAX_PTS 64

//GLOBAL page structues

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
static struct page pts[MAX_PTS][1024] __attribute__((aligned(4096)));
static int next_pt = 0;

//map pages function deliverable 
void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *root_pd) {
    uint32_t va = (uint32_t)vaddr;
    struct ppage *current = pglist;

    while(current != NULL) {
        //convert physical address to uint32_t
        uint32_t phys = (uint32_t)(uintptr_t)current->physical_addr;

        //check if its 4kb aligned
        if(phys & 0xFFF) return NULL;

        //figure out which directory entry the address uses
        uint32_t pd_index = (va >> 22) & 0x3FF;
        uint32_t pt_index = (va >> 12) & 0x3FF;

        struct page *pt;

        //check if there is a page table for this PD entry
        if (!root_pd[pd_index].present) {
            if(next_pt >= MAX_PTS) return NULL;

            pt = pts[next_pt++];
            memset(pt, 0, sizeof pts[0]);

            uint32_t pt_phys = (uint32_t)pt;

            struct page_directory_entry entry = {0};
            entry.present = 1;
            entry.rw = 1;
            entry.user = 0;
            entry.pagesize = 0;
            entry.frame = pt_phys >> 12;
            root_pd[pd_index] = entry;
        } else {
            //if the PD already has a table then we just need to find its address
            uint32_t pt_phys = root_pd[pd_index].frame << 12;
            pt = (struct page*)pt_phys; 
        }

        struct page pte = {0};
        pte.present = 1;
        pte.rw = 1;
        pte.user = 0;
        pte.frame = phys >> 12;
        pt[pt_index] = pte;

        //move to next page
        va += PAGE_SIZE;
        //increment ptr
        current = current->next;

    }
    return vaddr;

}

//provided assembly code

void loadPageDirectory(struct page_directory_entry *pd) {
    asm volatile("mov %0,%%cr3"
        :
        : "r"(pd)
        :);
}

void enable_paging(void) {
    asm volatile(
        "mov %%cr0, %%eax\n\t"
        "or  $0x80000001, %%eax\n\t"
        "mov %%eax, %%cr0\n\t"
        :
        :
        : "eax","memory");
}