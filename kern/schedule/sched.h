#ifndef INCLUDE_SCHEDULE_H
#define INCLUDE_SCHEDULE_H

#include "proc.h"

typedef enum {
    SCHED_RR = 0,        // Round Robin
    SCHED_FCFS = 1,      // First Come First Served
    SCHED_SJF = 2,       // Shortest Job First
    SCHED_PRIORITY = 3,  // Priority Scheduling
} sched_algorithm_t;

extern proc_struct_t *running_proc_head;
extern proc_struct_t *wait_proc_head;
extern proc_struct_t *current;

extern sched_algorithm_t current_sched_algorithm;

void init_sched();

void schedule();

void schedule_rr();          // Round Robin
void schedule_fcfs();         // First Come First Served
void schedule_sjf();          // Shortest Job First
void schedule_priority();     // Priority Scheduling

void set_sched_algorithm(sched_algorithm_t algo);

const char* get_sched_algorithm_name();

void change_task_to(proc_struct_t *next);

void switch_to(context_t *prev, context_t *next);

uint32_t read_eip();

#endif // INCLUDE_SCHEDULE_H
