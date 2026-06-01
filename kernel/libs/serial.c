#include "serial.h"

#define COM1 0x3F8

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    
    return val;
}

void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

static void serial_wait(void) {
    while (!(inb(COM1 + 5) & 0x20));
}

void serial_write_byte(char c) {
    serial_wait();
    outb(COM1, c);
}

void serial_write_string(const char* str) {
    while (*str) {
        serial_write_byte(*str++);
    }
}

void serial_write_uint(uint64_t n) {
    if (n == 0) { serial_write_string("0"); return; }

    char buf[20];
    int i = 19;
    buf[i] = '\0';

    while (n > 0) {
        buf[--i] = '0' + (n % 10);
        n /= 10;
    }
    serial_write_string(&buf[i]);
}

void serial_write_hex(uint64_t n) {
    const char *hex = "0123456789abcdef";
    char buf[19]; // "0x" + 16 digits + null
    buf[0] = '0'; buf[1] = 'x';
    for (int i = 0; i < 16; i++)
        buf[2 + i] = hex[(n >> (60 - i * 4)) & 0xF];
    buf[18] = '\0';
    serial_write_string(buf);
}
