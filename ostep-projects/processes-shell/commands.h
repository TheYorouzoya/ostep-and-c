#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "path.h"

/* enum keeps track of built-in command types */
typedef enum {
    CMD_EXIT,                   /* exits shell */
    CMD_CD,                     /* changes working directory */
    CMD_PATH,                   /* changes shell path variable */
    CMD_EXTERNAL                /* external command */
} command_t;

/**
 * Returns the given command string's type.
 **/
command_t get_command_type(char*);

void execute_built_in_command(command_t, char***, size_t, path_t*);

#endif // COMMANDS_H_
