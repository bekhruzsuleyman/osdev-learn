#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

typedef uint64_t pte_t;

typedef struct {
    pte_t entries[512];
} __attribute__((aligned(4096))) page_table_t;

void vmm_map(page_table_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_unmap(page_table_t* pml4, uint64_t virt);
void vmm_init();

#endif