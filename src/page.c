#include "page.h"
#include <stdint.h>
#include <stddef.h>

//2mib per page 
#define PAGE_SIZE (2 * 1024 * 1024)
#define NUM_PAGES 128

static struct ppage physical_page_array[NUM_PAGES];
static struct ppage *free_list = NULL;

//clear the next/previous pointers
static void clear_links(struct ppage* page) {
    page->next = NULL;
    page->prev = NULL;
}

//push a node to the front of the free list
static void push_front(struct ppage **head, struct ppage *node) {
    if(node == NULL)  {
        return;
    }

    node->prev = NULL;
    node->next = *head;

    if(*head != NULL) {
        (*head)->prev = node;
    }
    //new head is the node
    *head = node;

}

static struct ppage *pop_front(struct ppage **head) {
    struct ppage *n = *head;
    //null case
    if(n == NULL) {
        return NULL;
    }

    *head = n->next;

    if(*head != NULL) {
        (*head)->prev = NULL;
    }
    //clear links
    n->next = NULL;
    n->prev = NULL;
    return n;

}

//add the src list to the front of the dest list
static void add_list_front(struct ppage **dest, struct ppage *src_head) {
    struct ppage *tail;

    if(src_head == NULL) {
        return;
    }

    //set the tail as the src head
    tail = src_head;

    //go to the end of hte list
    while(tail->next != NULL) {
        tail = tail->next;
    }

    //now that tail is at the end connect them
    tail->next = *dest;

    if(*dest != NULL) {
        (*dest)->prev = tail;
    }

    *dest = src_head;
}

static struct ppage *get_head(struct ppage *p) {
    if(p == NULL) {
        return NULL;
    }
    //go back to the true head
    while(p->prev != NULL) {
        p = p->prev;
    }

    return p;
}

void init_pfa_list(void) {
    unsigned int i;

    free_list = NULL;

    for(i=0; i < NUM_PAGES; i++) {
        struct ppage *page = &physical_page_array[i];

        clear_links(page);

        //give each page its address
        page->physical_addr = (void *)((uintptr_t)i * (uintptr_t)PAGE_SIZE);

        //push it onto the free list
        push_front(&free_list, page);
    }
}

struct ppage *allocate_physical_pages(unsigned int num_pages) {
    struct ppage *head = NULL;
    struct ppage *tail = NULL;
    unsigned int i;

    //edge case if given 0 pages
    if(num_pages == 0) {
        return NULL;
    }

    for(i = 0; i < num_pages; i++) {
        struct ppage *node = pop_front(&free_list);

        if(node == NULL) {
            if(head != NULL) {
                //return pages to free list
                add_list_front(&free_list, head);
            }
            return NULL;
    }

        if(head == NULL) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }

    return head;
}

void free_physical_pages(struct ppage* page_list) {
    struct ppage *head;

    if (page_list == NULL) {
        return;
    }

    head = get_head(page_list);

    add_list_front(&free_list, head);
}