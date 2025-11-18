#ifndef INCLUDE_CMD_H
#define INCLUDE_CMD_H

#include "types.h"

#define MAX_CMD_LEN 256
#define MAX_ARGS 16

typedef int (*cmd_handler_t)(int argc, char **argv);

typedef struct {
    const char *name;
    const char *description;
    cmd_handler_t handler;
} cmd_t;

void init_cmd();

void register_cmd(const char *name, const char *description, cmd_handler_t handler);

int execute_cmd(const char *cmd_line);

int parse_cmd(char *cmd_line, char *argv[], int max_args);

#endif // INCLUDE_CMD_H

