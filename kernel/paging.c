#include <stdint.h>
#include "paging.h"
#include "memman.h"
#include "libs/kcpu.h"

#define PTE_PRESENT  (1ULL << 0)
#define PTE_WRITABLE (1ULL << 1)
#define PTE_USER     (1ULL << 2)

// [000000000] - PML4 [000000000] - PDPT [000000000] - PD [000000000] - PT [0000 0000 0000] - offset
#define PML4_INDEX(va) (((va) >> 39) & 0x1FFULL)
#define PDPT_INDEX(va) (((va) >> 30) & 0x1FFULL)
#define PD_INDEX(va) (((va) >> 21) & 0x1FFULL)
#define PT_INDEX(va) (((va) >> 12) & 0x1FFULL)

#define PTE_ADDR(entry) ((entry) & ~0xFFFULL)

static void invlpg(uint64_t virt) {
    __asm__ volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}

void vmm_map(page_table_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t table_flags = PTE_PRESENT | PTE_WRITABLE;
    if (flags & PTE_USER) table_flags |= PTE_USER;

    pte_t *pml4e = &pml4->entries[PML4_INDEX(virt)];
    if (!(*pml4e & PTE_PRESENT)) {
        uint64_t new_table = pmm_alloc();
        *pml4e = new_table | table_flags;
    }
    page_table_t *pdpt = (page_table_t*) PTE_ADDR(*pml4e);

    pte_t *pdpte = &pdpt->entries[PDPT_INDEX(virt)];
    if (!(*pdpte & PTE_PRESENT)) {
        uint64_t new_table = pmm_alloc();
        *pdpte = new_table | table_flags;
    }
    page_table_t *pd = (page_table_t*) PTE_ADDR(*pdpte);

    pte_t *pde = &pd->entries[PD_INDEX(virt)];
    if (!(*pde & PTE_PRESENT)) {
        uint64_t new_table = pmm_alloc();
        *pde = new_table | table_flags;
    }
    page_table_t *pt = (page_table_t*) PTE_ADDR(*pde);
    
    pte_t *pte = &pt->entries[PT_INDEX(virt)];
    *pte = phys | flags | PTE_PRESENT;

    invlpg(virt);
}

void vmm_unmap(page_table_t* pml4, uint64_t virt) {
    pte_t *pml4e = &pml4->entries[PML4_INDEX(virt)];
    if (!(*pml4e & PTE_PRESENT)) return;
    page_table_t *pdpt = (page_table_t*) PTE_ADDR(*pml4e);
    
    pte_t *pdpte = &pdpt->entries[PDPT_INDEX(virt)];
    if (!(*pdpte & PTE_PRESENT)) return;
    page_table_t *pd = (page_table_t*) PTE_ADDR(*pdpte);

    pte_t *pde = &pd->entries[PD_INDEX(virt)];
    if (!(*pde & PTE_PRESENT)) return;
    page_table_t *pt = (page_table_t*) PTE_ADDR(*pde);

    pte_t *pte = &pt->entries[PT_INDEX(virt)];
    if (!(*pte & PTE_PRESENT)) return;

    uint64_t phys = PTE_ADDR(*pte);
    *pte = 0;
    pmm_free(phys);

    invlpg(virt);
}

void vmm_init() {
    uint64_t phys = pmm_alloc();
    uint64_t flags = PTE_WRITABLE;

    page_table_t *pml4 = (page_table_t*) phys;
    for (uint64_t i = 0; i < 512; i++) pml4->entries[i] = 0;

    for (uint64_t addr = 0; addr < 0x400000; addr+=4096) {
        vmm_map(pml4, addr, addr, flags);
    }

    write_cr3(phys);
}