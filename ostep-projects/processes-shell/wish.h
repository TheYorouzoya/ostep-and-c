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


/**
 * Tokenizes the given line using strtok_r(),stores all the token pointers
 * into the provided tokens array, and returns the number of tokens encountered.
 *
 * On success, tokenize_line() returns the number of tokens encountered. Returns -1
 * on failure or if no tokens were found in the given line.
 *
 * The `tokens` pointer should be set to `NULL` before the call. Since the function
 * automatically resizes the array using realloc(), the caller should call
 * free() once they're done using the array.
 *
 * Since strtok_r() modifies the given string, make sure a copy of the line is passed.
 * The function uses whilespace `" "` as a delimiter while parsing the line for tokens.
 *
 * Also, since the pointers returned by strtok_r() all point to different sections
 * inside the input line, there is no need to free each individual entry within the
 * `tokens` array. Simply free the `lineptr` and the `tokens` pointers to reclaim
 * all memory.
 *
 **/
size_t tokenize_line(char ***, char**, const size_t);

void execute_command(char***, size_t, path_t*);

#endif // WISH_H_
