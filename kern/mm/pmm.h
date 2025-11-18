#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "multiboot.h"




extern uint8_t kern_start[];
extern uint8_t kern_end[];

#define STACK_SIZE    8192
#define KERN_STACK_SIZE 2048
#define PMM_MAX_SIZE  0x20000000
#define PMM_PAGE_SIZE 0x1000
#define PAGE_MAX_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)


extern uint32_t phy_page_count;


extern char kern_stack[];
extern uint32_t kern_stack_top;


void init_pmm();


uint32_t pmm_alloc_page();


void pmm_free_page(uint32_t p);


void show_memory_map();

#endif
