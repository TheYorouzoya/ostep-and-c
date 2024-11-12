#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdlib.h>
#include <string.h>
#include <bits/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "path.h"
#include "error.h"
#include "tokens.h"


command_type get_command_type(char*);

command_t *get_command(char*);

void free_command(command_t*);

void print_command(command_t*);

void execute_command(char*, path_t**);

void execute_built_in_command(command_t*, path_t**);

void execute_cd(char*);

path_t *execute_path(command_t*, path_t**);

void execute_external_command(command_t*, path_t*, char*);

void execute_commands_in_parallel(tokens_t*, path_t**);

char* isExecutable(command_t*, path_t*);


#endif // COMMANDS_H_
