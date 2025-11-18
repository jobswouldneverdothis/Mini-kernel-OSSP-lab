#include "sched.h"
#include "common.h"
#include "proc.h"
#include "pmm.h"
#include "heap.h"
#include "debug.h"
#include "gdt.h"

proc_struct_t *running_proc_head = NULL;
proc_struct_t *wait_proc_head = NULL;
proc_struct_t *current = NULL;
sched_algorithm_t current_sched_algorithm = SCHED_RR;
static uint32_t system_time = 0;

void init_sched()
{
    current = (proc_struct_t *)(kern_stack_top - STACK_SIZE);
    current->state = TASK_RUNNABLE;
    current->pid = now_pid++;
    set_proc_name(current, "idleproc");
    current->cr3 = (uint32_t)pgd_kern - PAGE_OFFSET;
    current->kstack = (uint32_t)kern_stack + STACK_SIZE;
    
    current->priority = 0;
    current->time_quantum = 10;
    current->runtime = 0;
    current->wait_time = 0;
    current->arrival_time = 0;
    current->burst_time = 0;

    current->next = current;
    current->prev = current;
    running_proc_head = current;
    
    current_sched_algorithm = SCHED_RR;
    system_time = 0;
}

void schedule()
{
    if (!current || !running_proc_head) {
        return;
    }
    
    system_time++;
    if (current != NULL) {
        current->runtime++;
    }
    
    proc_struct_t *p = running_proc_head;
    if (p) {
        do {
            if (p != current && p->state == TASK_RUNNABLE) {
                p->wait_time++;
            }
            p = p->next;
        } while (p != running_proc_head);
    }
    
    switch (current_sched_algorithm) {
        case SCHED_RR:
            schedule_rr();
            break;
        case SCHED_FCFS:
            schedule_fcfs();
            break;
        case SCHED_SJF:
            schedule_sjf();
            break;
        case SCHED_PRIORITY:
            schedule_priority();
            break;
        default:
            schedule_rr();
            break;
    }
}

void schedule_rr()
{
    if (current && current->next) {
        if (current->time_quantum > 0 && current->runtime >= current->time_quantum) {
            current->runtime = 0;
            change_task_to(current->next);
        }
    }
}

void schedule_fcfs()
{
    if (!current || !running_proc_head) {
        return;
    }
    
    proc_struct_t *next = NULL;
    proc_struct_t *p = running_proc_head;
    uint32_t earliest_time = 0xFFFFFFFF;
    
    do {
        if (p->state == TASK_RUNNABLE) {
            if (p->arrival_time < earliest_time) {
                earliest_time = p->arrival_time;
                next = p;
            }
        }
        p = p->next;
    } while (p != running_proc_head);
    
    if (next && next != current) {
        change_task_to(next);
    }
}

void schedule_sjf()
{
    if (!current || !running_proc_head) {
        return;
    }
    
    proc_struct_t *next = NULL;
    proc_struct_t *p = running_proc_head;
    uint32_t shortest_burst = 0xFFFFFFFF;
    
    do {
        if (p->state == TASK_RUNNABLE) {
            if (p->burst_time > 0 && p->burst_time < shortest_burst) {
                shortest_burst = p->burst_time;
                next = p;
            }
        }
        p = p->next;
    } while (p != running_proc_head);
    
    if (current->burst_time > 0 && current->runtime < current->burst_time) {
        return;
    }
    
    if (next && next != current) {
        change_task_to(next);
    }
}

void schedule_priority()
{
    if (!current || !running_proc_head) {
        return;
    }
    
    proc_struct_t *next = NULL;
    proc_struct_t *p = running_proc_head;
    uint32_t highest_priority = 0;
    
    do {
        if (p->state == TASK_RUNNABLE) {
            if (p->priority > highest_priority) {
                highest_priority = p->priority;
                next = p;
            }
        }
        p = p->next;
    } while (p != running_proc_head);
    
    if (next && next != current && next->priority > current->priority) {
        change_task_to(next);
    }
}

void set_sched_algorithm(sched_algorithm_t algo)
{
    if (algo >= SCHED_RR && algo <= SCHED_PRIORITY) {
        current_sched_algorithm = algo;
    }
}

const char* get_sched_algorithm_name()
{
    switch (current_sched_algorithm) {
        case SCHED_RR:
            return "Round Robin";
        case SCHED_FCFS:
            return "First Come First Served";
        case SCHED_SJF:
            return "Shortest Job First";
        case SCHED_PRIORITY:
            return "Priority Scheduling";
        default:
            return "Unknown";
    }
}

void change_task_to(proc_struct_t *next)
{
    if (current != next && next != NULL) {
        proc_struct_t *prev = current;
        current = next;
        set_kernel_stack(next->kstack);
        switch_pgd(next->cr3);
        switch_to(&(prev->context), &(current->context));
    }
}
