#ifndef KCPU_H
#define KCPU_H

#include <stdint.h>

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpu_regs_t;

extern void cpuid_call(
    uint32_t leaf,
    uint32_t subleaf,
    cpu_regs_t *regs
);

static inline void cpu_vendor(char vendor[13]) {
    cpu_regs_t regs;

    cpuid_call(0, 0, &regs);

    *(uint32_t *) (vendor + 0) = regs.ebx;
    *(uint32_t *) (vendor + 4) = regs.edx;
    *(uint32_t *) (vendor + 8) = regs.ecx;
    vendor[12] = '\0';
};

#endif