#ifndef TOKENS_H_
#define TOKENS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "structs.h"
#include "error.h"

void free_tokens(tokens_t*);

tokens_t *tokenize_line(char **, size_t);

int parse_redirect(char*, char**);

void print_tokens(tokens_t*);

#endif // TOKENS_H_
