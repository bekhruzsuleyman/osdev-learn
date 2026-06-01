#ifndef MEMMAN_H
#define MEMMAN_H

#include <stdint.h>
#include <stddef.h>
#include "../limine.h"

void memman_init(struct limine_memmap_response *response);

uint64_t pmm_alloc();

void pmm_free(uint64_t phys_addr);

uint64_t get_total_memory(struct limine_memmap_response *memmap);

uint64_t get_free_memory(void);

#endif