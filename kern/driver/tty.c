#include "types.h"
#include "keyboard.h"
#include "debug.h"
#include "cmd.h"
#include "string.h"

#define CMD_BUFFER_SIZE 256

static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_buffer_pos = 0;
static int prompt_shown = 0;

static void show_prompt()
{
    printk("mini-os> ");
    prompt_shown = 1;
}

int task_tty(void *args)
{
    init_cmd();
    
    show_prompt();
    
    while (1) {
        keyboard_read();
    }
    return 0;
}

void in_process(uint32_t key)
{
    char output[2] = {'\0', '\0'};
    
    if (key & FLAG_EXT) {
        if (key == ENTER || key == PAD_ENTER) {
            if (cmd_buffer_pos > 0) {
                cmd_buffer[cmd_buffer_pos] = '\0';
                printk("\n");
                
                execute_cmd(cmd_buffer);
                
                cmd_buffer_pos = 0;
                bzero(cmd_buffer, CMD_BUFFER_SIZE);
            } else {
                printk("\n");
            }
            
            show_prompt();
            return;
        } else if (key == BACKSPACE) {
            if (cmd_buffer_pos > 0) {
                cmd_buffer_pos--;
                cmd_buffer[cmd_buffer_pos] = '\0';
                printk("\b \b");
            }
            return;
        }
    }
    
    if (!(key & FLAG_EXT)) {
        char c = key & 0xFF;
        
        if (c >= ' ' && c <= '~') {
            if (cmd_buffer_pos < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_buffer_pos++] = c;
                cmd_buffer[cmd_buffer_pos] = '\0';
                output[0] = c;
                printk("%s", output);
            }
        }
    }
}

