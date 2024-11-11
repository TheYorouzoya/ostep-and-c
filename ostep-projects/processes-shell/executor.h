#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "path.h"
#include "error.h"
#include "commands.h"


void execute_external_command(command_t*, path_t*);

void execute_command_and_redirect(command_t*, path_t*, char*);


/**
 * Checks whether the given command is executable within the given path variable's
 * paths. Returns the index of the path where the process is executable. For example,
 * if the paths are '/usr/bin' and '/bin' and the given command is executable in the
 * first path, the function will return 0 (i.e., the index of the path within the
 * path_t struct variable).
 *
 * On error, or if the command isn't executable in any of the paths, returns -1.
 **/
char* isExecutable(command_t*, path_t*);

#endif // EXECUTOR_H_
