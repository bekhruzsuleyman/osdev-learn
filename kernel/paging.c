#include <stdint.h>
#include "paging.h"
#include "memman.h"

#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER     (1ULL << 2)

// [000000000] - PML4 [000000000] - PDPT [000000000] - PD [000000000] - PT [0000 0000 0000] - offset
#define PML4_INDEX(va) (((va) >> 39) & 0x1FFULL)
#define PDPT_INDEX(va) (((va) >> 30) & 0x1FFULL)
#define PD_INDEX(va) (((va) >> 21) & 0x1FFULL)
#define PT_INDEX(va) (((va) >> 12) & 0x1FFULL)

#define PTE_ADDR(entry) ((entry) & ~0xFFFULL)

void vmm_map(page_table_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    pte_t *pml4e = &pml4->entries[PML4_INDEX(virt)];
    if (!(*pml4e & PTE_PRESENT)) {
        uint64_t table = pmm_alloc();
        *pml4e = table | PTE_PRESENT | PTE_WRITABLE;
    }
    page_table_t *pdpt = (page_table_t*) PTE_ADDR(*pml4e);

    pte_t *pdpte = &pdpt->entries[PDPT_INDEX(virt)];
    if (!(*pdpte & PTE_PRESENT)) {
        uint64_t table = pmm_alloc();
        *pdpte = table | PTE_PRESENT | PTE_WRITABLE;
    }
    page_table_t *pd = (page_table_t*) PTE_ADDR(*pdpte);
    
    pte_t *pde = &pd->entries[PD_INDEX(virt)];
    if (!(*pde & PTE_PRESENT)) {
        uint64_t table = pmm_alloc();
        *pde = table | PTE_PRESENT | PTE_WRITABLE;
    }
    page_table_t *pt = (page_table_t*) PTE_ADDR(*pde);

    pte_t *pte = &pt->entries[PT_INDEX(virt)];    
    *pte = phys | flags | PTE_PRESENT;
}