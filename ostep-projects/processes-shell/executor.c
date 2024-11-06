#include "executor.h"
#include <stdlib.h>


void execute_external_command(char ***tokens, size_t token_num, path_t *path) {

    /* TODO: A parser or preprocessor to parse redirection and parallel commands */

    char *command = *tokens[0];
    int wstatus;

    if ((command = isExecutable(command, path)) < 0) {
        print_error();
        return;
    }

    /* command is executable now */

    pid_t proc = fork();        /* fork and execute command */

    if (proc == -1) {
        /* fork failed */
        print_error();
        return;
    }

    if (proc == 0) {
        /* execute command in child */

        /* resize token list to include terminating NULL pointer for execv() */
        char **args = malloc(sizeof(char **) * (token_num + 1));

        if (!args) {
            /* malloc failed */
            print_error();
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < token_num; i++) {
            args[i] = *tokens[i];
        }

        args[token_num] = NULL;

        execv(command, *tokens);

        /* exec returning here means an error occured */
        print_error();
        free(args);
        exit(EXIT_FAILURE);

    } else {
        /* shell waits for the command to finish executing */
        proc = waitpid(proc, &wstatus, 0);
    }
}


char* isExecutable(char *command, path_t *path) {
    char *full_path, *curr;

    /* traverse each path, append command to path string and check if executable */
    for (int i = 0, cmd_len = strlen(command); i < path->num; i++) {
        curr = path->paths[i];  /* current path */
        int p_len = strlen(curr);

        full_path = malloc(p_len + cmd_len + 2); /* initialize appended string */
        if (!full_path) {
            /* malloc failed */
            return NULL;
        }

        full_path[0] = '\0';

        strncat(full_path, curr, p_len); /* copy current path */

        full_path[p_len] = '/'; /* add '/' before appending command */
        full_path[p_len + 1] = '\0';

        strncat(full_path, command, cmd_len); /* append command */

        if (access(full_path, X_OK) == 0) {
            /* command is executable */
            command = full_path;
            return command;
        }

        free(full_path);
    }

    return NULL;
}
