#include "memman.h"
#include "serial.h"

#define FRAME_SIZE 4096

uint64_t get_total_memory(struct limine_memmap_response *memmap) {
    uint64_t entry_count = memmap->entry_count;
    struct limine_memmap_entry **entries = memmap->entries;

    uint64_t total_memory = 0;
    for (uint64_t i = 0; i < entry_count; i++) total_memory += entries[i]->length;

    return total_memory;
}

static uint64_t get_usable_addr(struct limine_memmap_response *memmap) {
    uint64_t entry_count = memmap->entry_count;
    struct limine_memmap_entry **entries = memmap->entries;

    for (uint64_t i = 0; i < entry_count; i++) {
        if (entries[i]->type == LIMINE_MEMMAP_USABLE) return entries[i]->base;
    }

    return 0;
}

uint64_t BITMAP_SIZE;
static uint8_t *bitmap = NULL;

static inline int bitmap_get(uint64_t page) {
    uint64_t byte = page / 8;
    uint64_t bit = page % 8;
    return (bitmap[byte] >> bit) & 1ULL;
}

static inline void bitmap_set(uint64_t page) {
    uint64_t byte = page / 8;
    uint64_t bit = page % 8;
    bitmap[byte] |= (1ULL << bit);
}

static inline void bitmap_clear(uint64_t page) {
    uint64_t byte = page / 8;
    uint64_t bit = page % 8;
    bitmap[byte] &= ~(1ULL << bit);
}

void bitmap_init(struct limine_memmap_response *memmap) {
    uint64_t total_memory = get_total_memory(memmap);
    BITMAP_SIZE = total_memory / FRAME_SIZE;

    uint64_t first_usable_base = get_usable_addr(memmap);
    bitmap = (uint8_t*)first_usable_base;

    for (uint64_t i = 0; i < BITMAP_SIZE / 8; i++) bitmap[i] = 0;

    uint64_t bitmap_bytes = BITMAP_SIZE / 8;
    uint64_t bitmap_pages = (bitmap_bytes + FRAME_SIZE - 1) / FRAME_SIZE;
    uint64_t bitmap_pages_base = first_usable_base / FRAME_SIZE;

    for (uint64_t i = 0; i < bitmap_pages; i++) {
        bitmap_set(bitmap_pages_base + i);
    }
}

static uint64_t total_pages = 0;
static uint64_t used_pages = 0;
static uint64_t highest_pages = 0;

void memman_init(struct limine_memmap_response *memmap) {
    serial_write_string("Initializing memory manager ...\n");
    
    uint64_t entry_count = memmap->entry_count;
    
	for (uint64_t i = 0; i < entry_count; i++) {
		struct limine_memmap_entry *entry = memmap->entries[i];

        uint64_t page_base = entry->base / FRAME_SIZE;
        uint64_t page_count = entry->length / FRAME_SIZE;

		serial_write_string("Entry ");
		serial_write_uint(i);
		serial_write_string(": Pages ");
		serial_write_hex(page_base);
		serial_write_string(" - ");
		serial_write_hex(page_base + page_count);
		serial_write_string(" (");
		serial_write_uint(page_count * 4);
		serial_write_string(" KB) Type: ");
		serial_write_uint(entry->type);
		serial_write_string("\n");
        
        if (entry->type != LIMINE_MEMMAP_USABLE) {
            for (uint64_t j = 0; j < page_count; j++) {
                if ((page_base + j) < BITMAP_SIZE) bitmap_set(page_base + j);
                used_pages++;
            }
        } else {
            total_pages += page_count;
            uint64_t top = page_base + page_count;
            if (top > highest_pages) highest_pages = top;
        }
	}
}

uint64_t pmm_alloc() {
    for (uint64_t byte = 0; byte < BITMAP_SIZE / 8; byte++) {
        if (bitmap[byte] == 0xFF) continue;
        for (uint64_t bit = 0; bit < 8; bit++) {
            uint64_t page = byte * 8 + bit;
            if (!bitmap_get(page)) {
                bitmap_set(page);
                used_pages++;
                return page * FRAME_SIZE;
            }
        }
    }
    return 0;
}

void pmm_free(uint64_t phys_addr) {
    uint64_t page = phys_addr / FRAME_SIZE;
    if (page >= BITMAP_SIZE) return;
    if(!bitmap_get(page)) return;
    bitmap_clear(page);
    used_pages--;
}