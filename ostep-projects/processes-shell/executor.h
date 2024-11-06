#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "path.h"
#include "error.h"


void execute_external_command(char***, size_t, path_t*);


/**
 * Checks whether the given command is executable within the given path variable's
 * paths. Returns the index of the path where the process is executable. For example,
 * if the paths are '/usr/bin' and '/bin' and the given command is executable in the
 * first path, the function will return 0 (i.e., the index of the path within the
 * path_t struct variable).
 *
 * On error, or if the command isn't executable in any of the paths, returns -1.
 **/
char* isExecutable(char*, path_t*);

#endif // EXECUTOR_H_
