#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "path.h"
#include "error.h"

/* enum keeps track of built-in command types */

/**
 * Returns the given command string's type.
 **/
command_type get_command_type(char*);

command_t *get_command(char*, size_t);

void free_command(command_t*);

void print_command(command_t*);

void execute_built_in_command(command_t*, path_t**);

void execute_cd(char*);

path_t *execute_path(command_t*, path_t**);

#endif // COMMANDS_H_
