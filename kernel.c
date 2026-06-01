#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include "limine.h"
#include "kernel/framebuffer.h"
#include "kernel/libs/types.h"
#include "kernel/libs/serial.h"

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST_ID,
	.revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
	.revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST_ID,
	.revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

static inline noreturn void hlt(void) {
	for (;;) {
		__asm__ volatile ("hlt");
	}
}

void kmain(void) {
	serial_init();

	serial_write_string("Kernel Loaded!\n");

	if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
		hlt();
	}

	// Memory Map

	if (memmap_request.response == NULL) {
		serial_write_string("No Memory Map Response!\n");
		hlt();
	}

	struct limine_memmap_response *memmap = memmap_request.response;

	// Framebuffer

	if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count == 0) {
		serial_write_string("No FrameBuffer Response!\n");
		hlt();
	}

	uint64_t framebuffer_count = framebuffer_request.response->framebuffer_count;
	uint32_t green = color_rgba(0, 255, 0, 255);
	uint32_t red = color_rgba(255, 0, 0, 255);
	uint32_t blue = color_rgba(0, 0, 255, 255);

	struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
	volatile uint32_t *pixel = (uint32_t *) fb->address;
	for (uint64_t i = 0; i < (fb->pitch / 4) * fb->height; i++) {
		pixel[i] = green;
	}

	Position2D p1 = {100, 30};

	draw_pixel(fb, p1, red);

	Position2D positionStart;
	Position2D positionEnd;
	positionStart.x = 50;
	positionStart.y = 50;

	positionEnd.x = 150;
	positionEnd.y = 150;

	draw_rectangle(fb, positionStart, positionEnd, blue);

	hlt();
}
