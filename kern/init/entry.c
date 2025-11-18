#include "common.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "clock.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "sched.h"
#include "keyboard.h"
#include "tty.h"


void kern_init();


multiboot_t *glb_mboot_ptr;


char kern_stack[STACK_SIZE];
uint32_t kern_stack_top;


__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_hign = (pgd_t *)0x3000;


__attribute__((section(".init.text"))) void kern_entry()
{
    pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
    pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_hign | PAGE_PRESENT | PAGE_WRITE;


    for (int i = 0; i < 1024; i++) {
        pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }


    for (int i = 0; i < 1024; i++) {
        pte_hign[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }


    asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

    uint32_t cr0;


    asm volatile ("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" : : "r" (cr0));


    kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
    asm volatile ("mov %0, %%esp\n\t"
            "xor %%ebp, %%ebp" : : "r" (kern_stack_top));


    glb_mboot_ptr = (multiboot_t *)((uint32_t)tmp_mboot_ptr + PAGE_OFFSET);


    kern_init();
}


void kern_init()
{

    init_debug();
    console_clear();

    init_gdt();

    init_idt();

    init_timer(100);

    init_pmm();

    init_vmm();

    init_heap();

    init_sched();

    init_keyboard();


    printk_color(rc_black, rc_red, "\nfree physical memory: %u MB\n", phy_page_count * 4 / 1024);


    printk_color(rc_black, rc_magenta, "\ntest kmalloc() && kfree() now ...\n\n");

    void *addr2 = kmalloc(500);
    printk("kmalloc   500 byte in 0x%X\n", addr2);
    void *addr3 = kmalloc(5000);
    printk("kmalloc  5000 byte in 0x%X\n", addr3);
    void *addr4 = kmalloc(50000);
    printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

    printk("free mem in 0x%X\n", addr2);
    kfree(addr2);
    printk("free mem in 0x%X\n", addr3);
    kfree(addr3);
    printk("free mem in 0x%X\n\n", addr4);
    kfree(addr4);


    kernel_thread(task_tty, NULL);


    sti();
    while(1) {
        schedule();
    }
}
