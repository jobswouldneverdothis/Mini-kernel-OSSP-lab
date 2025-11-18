#include "proc.h"
#include "sched.h"
#include "pmm.h"
#include "vmm.h"
#include "gdt.h"
#include "heap.h"
#include "debug.h"
#include "string.h"
#include "common.h"

pid_t now_pid = 0;

int32_t kernel_thread(int (*fn)(void *), void *arg)
{
    proc_struct_t *new_proc = (proc_struct_t *)kmalloc(STACK_SIZE);
    assert(new_proc != NULL, "kernel_thread : kmalloc error\n");

    bzero(new_proc, sizeof(proc_struct_t));

    new_proc->cr3   = (uint32_t)pgd_kern - PAGE_OFFSET;
    new_proc->state = TASK_RUNNABLE;
    new_proc->pid   = now_pid++;
    set_proc_name(new_proc, "");
    new_proc->kstack = (uint32_t)new_proc + STACK_SIZE;
    
    new_proc->priority = 5;
    new_proc->time_quantum = 10;
    new_proc->runtime = 0;
    new_proc->wait_time = 0;
    new_proc->arrival_time = now_pid;
    new_proc->burst_time = 20;

    uint32_t *stack_top = (uint32_t *)((uint32_t)new_proc + STACK_SIZE);
    *(--stack_top) = (uint32_t)arg;
    *(--stack_top) = (uint32_t)kthread_exit;
    *(--stack_top) = (uint32_t)fn;

    new_proc->context.esp = (uint32_t)new_proc + STACK_SIZE - 3 * sizeof(uint32_t);
    new_proc->context.eflags = 0x200;

    new_proc->next = running_proc_head;
    new_proc->prev = running_proc_head->prev;
    running_proc_head->prev->next = new_proc;
    running_proc_head->prev = new_proc;

    return new_proc->pid;
}

void kthread_exit()
{
    register uint32_t val asm("eax");
    printk("Thread exited with value %d", val);
}

char * set_proc_name(struct proc_struct *proc, const char *name) {
    bzero(proc->name, sizeof(proc->name));
    return strcpy(proc->name, name);
}

int32_t getpid()
{
    return current->pid;
}
