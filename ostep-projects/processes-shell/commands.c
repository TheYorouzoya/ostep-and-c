#include "commands.h"

#include <string.h>

command_t get_command_type(char *command) {
    if(strcmp(command, "exit") == 0) return CMD_EXIT;
    if(strcmp(command, "cd") == 0) return CMD_CD;
    if(strcmp(command, "path") == 0) return CMD_PATH;

    return CMD_EXTERNAL;
}


void execute_built_in_command(command_t cmd_type, char ***tokens, size_t token_num, path_t *path) {

}
