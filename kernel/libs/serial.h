#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(void);
void serial_write_byte(char c);
void serial_write_string(const char* str);
void serial_write_uint(uint64_t n);
void serial_write_hex(uint64_t n);

#endif