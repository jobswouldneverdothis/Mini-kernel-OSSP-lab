#include "cmd.h"
#include "string.h"
#include "debug.h"
#include "sched.h"
#include "proc.h"
#include "console.h"

#define MAX_CMDS 32

static cmd_t cmd_table[MAX_CMDS];
static int cmd_count = 0;

static int cmd_help(int argc, char **argv);
static int cmd_clear(int argc, char **argv);
static int cmd_ps(int argc, char **argv);
static int cmd_sched(int argc, char **argv);
static int cmd_test(int argc, char **argv);

void init_cmd()
{
    cmd_count = 0;
    bzero(cmd_table, sizeof(cmd_table));
    
    register_cmd("help", "Show available commands", cmd_help);
    register_cmd("clear", "Clear the screen", cmd_clear);
    register_cmd("ps", "List all processes", cmd_ps);
    register_cmd("sched", "Set/get scheduling algorithm (usage: sched [rr|fcfs|sjf|priority])", cmd_sched);
    register_cmd("test", "Create a test process", cmd_test);
}

void register_cmd(const char *name, const char *description, cmd_handler_t handler)
{
    if (cmd_count >= MAX_CMDS) {
        return;
    }
    
    cmd_table[cmd_count].name = name;
    cmd_table[cmd_count].description = description;
    cmd_table[cmd_count].handler = handler;
    cmd_count++;
}

int parse_cmd(char *cmd_line, char *argv[], int max_args)
{
    int argc = 0;
    char *p = cmd_line;
    char *arg_start = NULL;
    int in_arg = 0;
    
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    
    if (*p == '\0') {
        return 0;
    }
    
    while (*p != '\0' && argc < max_args) {
        if (*p == ' ' || *p == '\t') {
            if (in_arg) {
                *p = '\0';
                in_arg = 0;
            }
        } else {
            if (!in_arg) {
                arg_start = p;
                argv[argc++] = arg_start;
                in_arg = 1;
            }
        }
        p++;
    }
    
    return argc;
}

int execute_cmd(const char *cmd_line)
{
    if (!cmd_line || strlen(cmd_line) == 0) {
        return 0;
    }
    
    static char cmd_buf[MAX_CMD_LEN];
    int len = strlen(cmd_line);
    if (len >= MAX_CMD_LEN) {
        len = MAX_CMD_LEN - 1;
    }
    memcpy((uint8_t *)cmd_buf, (const uint8_t *)cmd_line, len);
    cmd_buf[len] = '\0';
    
    if (len > 0 && (cmd_buf[len-1] == '\n' || cmd_buf[len-1] == '\r')) {
        cmd_buf[len-1] = '\0';
        len--;
    }
    
    char *argv[MAX_ARGS];
    int argc = parse_cmd(cmd_buf, argv, MAX_ARGS);
    
    if (argc == 0) {
        return 0;
    }
    
    for (int i = 0; i < cmd_count; i++) {
        if (strcmp(cmd_table[i].name, argv[0]) == 0) {
            return cmd_table[i].handler(argc, argv);
        }
    }
    
    printk("Command not found: %s\n", argv[0]);
    printk("Type 'help' for available commands.\n");
    return -1;
}

static int cmd_help(int argc, char **argv)
{
    printk("\nAvailable commands:\n");
    printk("==================\n");
    for (int i = 0; i < cmd_count; i++) {
        printk("  %-10s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
    printk("\n");
    return 0;
}

static int cmd_clear(int argc, char **argv)
{
    console_clear();
    return 0;
}

static int cmd_ps(int argc, char **argv)
{
    printk("\nPID  Name            State      Priority  Runtime  Wait Time\n");
    printk("------------------------------------------------------------\n");
    
    if (running_proc_head) {
        proc_struct_t *p = running_proc_head;
        do {
            const char *state_str = "UNKNOWN";
            switch (p->state) {
                case TASK_UNINIT:
                    state_str = "UNINIT";
                    break;
                case TASK_SLEEPING:
                    state_str = "SLEEPING";
                    break;
                case TASK_RUNNABLE:
                    state_str = "RUNNABLE";
                    break;
                case TASK_ZOMBIE:
                    state_str = "ZOMBIE";
                    break;
            }
            
            printk("%-4d %-15s %-10s %-8d %-8d %-8d\n",
                   p->pid,
                   p->name[0] ? p->name : "(unnamed)",
                   state_str,
                   p->priority,
                   p->runtime,
                   p->wait_time);
            p = p->next;
        } while (p != running_proc_head);
    }
    
    printk("\nCurrent scheduling algorithm: %s\n", get_sched_algorithm_name());
    printk("\n");
    return 0;
}

static int cmd_sched(int argc, char **argv)
{
    if (argc == 1) {
        printk("Current scheduling algorithm: %s\n", get_sched_algorithm_name());
        return 0;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "rr") == 0) {
            set_sched_algorithm(SCHED_RR);
            printk("Scheduling algorithm set to: Round Robin\n");
        } else if (strcmp(argv[1], "fcfs") == 0) {
            set_sched_algorithm(SCHED_FCFS);
            printk("Scheduling algorithm set to: First Come First Served\n");
        } else if (strcmp(argv[1], "sjf") == 0) {
            set_sched_algorithm(SCHED_SJF);
            printk("Scheduling algorithm set to: Shortest Job First\n");
        } else if (strcmp(argv[1], "priority") == 0) {
            set_sched_algorithm(SCHED_PRIORITY);
            printk("Scheduling algorithm set to: Priority Scheduling\n");
        } else {
            printk("Invalid algorithm. Use: rr, fcfs, sjf, or priority\n");
            return -1;
        }
        return 0;
    }
    
    printk("Usage: sched [rr|fcfs|sjf|priority]\n");
    return -1;
}

static int test_task(void *arg)
{
    int count = 0;
    int task_id = (int)(uint32_t)arg;
    while (count < 1000) {
        count++;
        for (volatile int i = 0; i < 10000; i++);
    }
    printk("Test process %d completed\n", task_id);
    return 0;
}

static int cmd_test(int argc, char **argv)
{
    static int test_count = 0;
    test_count++;
    
    pid_t pid = kernel_thread(test_task, (void *)(uint32_t)test_count);
    if (running_proc_head && running_proc_head->prev) {
        set_proc_name(running_proc_head->prev, "test_proc");
    }
    printk("Created test process with PID: %d\n", pid);
    return 0;
}

