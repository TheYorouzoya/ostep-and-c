#ifndef WISH_H_
#define WISH_H_

/* Library Headers */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* External headers */
#include "path.h"
#include "commands.h"
#include "executor.h"
#include "error.h"

void print_error();

void run_batch_mode();

void run_interactive_mode();

command_t *tokenize_line(char**, const size_t);

int parse_redirect(char*, size_t, char**);

bool check_syntax(command_t*, int, char**);

void execute_command(command_t*, path_t**);

#endif // WISH_H_
