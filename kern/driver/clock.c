#include "clock.h"
#include "idt.h"
#include "common.h"
#include "debug.h"
#include "sched.h"


void timer_callback(pt_regs_t *regs)
{
    schedule();
}

void init_timer(uint32_t frequency)
{
    register_interrupt_handler(32, timer_callback);


    uint32_t divisor = (1193182  + frequency / 2) / frequency;


    outb(0x43, 0x36);

    outb(0x40, (uint8_t)(divisor % 256));
    outb(0x40, (uint8_t)(divisor / 256));
}
