#ifndef PATH_H_
#define PATH_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct path_s path_t;

/**
 * Struct holds the number of paths and all path strings
 **/
struct path_s {
    char **paths;
    size_t num;
};


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


/**
 * Overwrites the given `path_t`'s paths with the paths contained in the provided
 * `paths` array and returns the number of paths added.
 *
 * Remember that the path command always overwrites the path variable so this
 * behavior is expected.
 *
 * If the given path list is empty, the paths variable in the returned struct isn't
 * initialized/allocated.
 *
 * This function returns -1 if an error occurs (for e.g., malloc() fails). In case of
 * an error, the provided `path_t` variable remains untouched.
 * */
size_t add_paths(path_t*, char***, const size_t);


/* print a given path variable's contents for debugging */
void print_paths(path_t*);

#endif // PATH_H_
