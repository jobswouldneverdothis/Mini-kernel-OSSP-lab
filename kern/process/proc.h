#ifndef INCLUDE_PROC_H
#define INCLUDE_PROC_H

#include "types.h"
#include "vmm.h"

#define PROC_NAME_LEN               15
#define MAX_PROCESS                 4096
#define MAX_PID                     (MAX_PROCESS * 2)

typedef
enum task_state {
    TASK_UNINIT = 0,
    TASK_SLEEPING = 1,
    TASK_RUNNABLE = 2,
    TASK_ZOMBIE = 3,
} task_state_t;

typedef
struct context {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
} context_t;

typedef
struct proc_struct {
    volatile task_state_t state;
    volatile bool need_resched;
    context_t context;
    uint32_t kstack;
    uint32_t cr3;
    pid_t pid;
    char name[PROC_NAME_LEN + 1];
    uint32_t flags;
    struct proc_struct *parent;

    uint32_t priority;
    uint32_t time_quantum;
    uint32_t runtime;
    uint32_t wait_time;
    uint32_t arrival_time;
    uint32_t burst_time;

    struct proc_struct *prev;
    struct proc_struct *next;
} proc_struct_t;


extern pid_t now_pid;

int32_t kernel_thread(int (*fn)(void *), void *arg);

void kthread_exit();

char *set_proc_name(struct proc_struct *proc, const char *name);

int32_t getpid();

#endif // INCLUDE_PROC_H
