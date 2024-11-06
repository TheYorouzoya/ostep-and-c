#ifndef WISH_H_
#define WISH_H_

#include <stdlib.h>


#define ERROR_MESSAGE "An error has occured\n"
#define PROMPT "wish> "

void print_error();

void run_batch_mode();

void run_interactive_mode();

size_t tokenize_line();


#endif // WISH_H_
