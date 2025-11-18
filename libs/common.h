#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#include "types.h"

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void sti(void);
void cli(void);
uint32_t read_eflags(void);
void write_eflags(uint32_t eflags);
void invlpg(void *addr);

#endif // INCLUDE_COMMON_H
