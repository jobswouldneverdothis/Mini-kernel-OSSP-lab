#include "vmm.h"
#include "debug.h"
#include "pmm.h"
#include "string.h"
#include "heap.h"


pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));


static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));


void init_vmm()
{
    uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);


    for (uint32_t i = kern_pte_first_idx, j = 0; j < PTE_COUNT; i++, j++) {

        pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
    }

    pte_t* pte = (pte_t *)pte_kern;

    for (uint32_t i = 1; i < PTE_COUNT * PTE_SIZE; i++) {
        pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
    }


    register_interrupt_handler(14, page_fault);


    uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;
    switch_pgd(pgd_kern_phy_addr);
}


void switch_pgd(uint32_t pd)
{

    asm volatile ("mov %0, %%cr3" : : "r"(pd));
}


void page_fault(pt_regs_t *regs)
{

    uint32_t cr2;
    asm volatile ("mov %%cr2, %0" : "=r"(cr2));
    printk("Page fault at 0x%x, virtual address 0x%x\n", regs->eip, cr2);
    printk("Error code : %x\n", regs->err_code);


    if (!(regs->err_code & 0x1)) {
        printk_color(rc_black, rc_red, "The page wasn't present.\n");
    }


    if (regs->err_code & 0x2) {
        printk_color(rc_black, rc_red, "Write error.\n");
    } else {
        printk_color(rc_black, rc_red, "Read error.\n");
    }


    if (regs->err_code & 0x4) {
        printk_color(rc_black, rc_red, "User mode.\n");
    } else {
        printk_color(rc_black, rc_red, "Kernel mode.\n");
    }


    if (regs->err_code & 0x8) {
        printk_color(rc_black, rc_red, "Reserved bits being overwritten.\n");
    }


    if (regs->err_code & 0x10) {
        printk_color(rc_black, rc_red, "The fault occurred during an instruction fetch.\n");
    }

    while (1);
}


void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);


    pte_t *pte = (pte_t *)(ROUNDDOWN(pgd_now[pgd_idx], PAGE_SIZE));
    if (!pte) {

        pte = (pte_t *)pmm_alloc_page();
        pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;


        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
        bzero(pte, PAGE_SIZE);
    } else {

        pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
    }


    pte[pte_idx] = (ROUNDDOWN(pa, PAGE_SIZE) | flags);

    asm volatile ("invlpg (%0)" : : "a"(va));
}


void unmap(pgd_t *pgd_now, uint32_t va)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);


    pte_t *pte = (pte_t *)(ROUNDDOWN(pgd_now[pgd_idx], PAGE_SIZE));
    if (!pte) {
        return;
    }


    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);


    pte[pte_idx] = 0;

    asm volatile ("invlpg (%0)" : : "a"(va));
}


uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa)
{
    uint32_t pgd_idx = PGD_INDEX(va);
    uint32_t pte_idx = PTE_INDEX(va);


    pte_t *pte = (pte_t *)(ROUNDDOWN(pgd_now[pgd_idx], PAGE_SIZE));
    if (!pte) {
        return 0;
    }


    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

    if (pte[pte_idx] != 0 && pa) {
        *pa = ROUNDDOWN(pte[pte_idx], PAGE_SIZE);
        return 1;
    }

    return 0;
}
