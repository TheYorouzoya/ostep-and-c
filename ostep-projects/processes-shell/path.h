#ifndef PATH_H_
#define PATH_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "structs.h"


/**
 * Initializes and returns a default path variable for the shell.
 * The default path only includes `/bin` as a path.
 **/
path_t* init_default_path();


/**
 * Frees the given path and all the path strings within it.
 * This function will throw an error if the number of paths in the p->num
 * doesn't match up with the actual path strings stored in the struct.
 * */
void free_path(path_t*);


path_t *add_paths(command_t*);


/* print a given path variable's contents for debugging */
void print_paths(path_t*);

#endif // PATH_H_
