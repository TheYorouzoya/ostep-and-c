#include "executor.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void execute_external_command(command_t *cmd, path_t *path) {
    int wstatus;

    char *exec_path = isExecutable(cmd, path);

    if (exec_path == NULL) {
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

        execv(exec_path, cmd->args);

        /* exec returning here means an error occured */
        printf("Execv failed\n");
        print_error();
        exit(EXIT_FAILURE);

    } else {
        /* shell waits for the command to finish executing */
        proc = waitpid(proc, &wstatus, 0);
        free(exec_path);
    }
}


void execute_command_and_redirect(command_t *cmd, path_t *path, char *redir_file) {
    /* check if command is executable */
    int wstatus;

    char *exec_path = isExecutable(cmd, path);

    if (exec_path == NULL) {
        /* command not executable */
        print_error();
        return;
    }

    pid_t proc = fork();

    if (proc == -1) {
        /* fork failed */
        print_error();
        return;
    }

    if (proc == 0) {
        /* execute command in child */
        FILE *fp = freopen(redir_file, "w+", stdout);

        if (dup2(fileno(stdout), fileno(stderr)) == -1) {
            fclose(fp);
            print_error();
            exit(EXIT_FAILURE);
        }

        /* execute command */
        execv(exec_path, cmd->args);

        /* exec returning means an error occured */
        print_error();
        fclose(fp);
        exit(EXIT_FAILURE);

    } else {
        /* shell waits for the command to finish executing */
        proc = waitpid(proc, &wstatus, 0);
        free(exec_path);
    }
}


char* isExecutable(command_t *cmd, path_t *path) {
    char *full_path, *curr;

    /* traverse each path, append command to path string and check if executable */
    for (int i = 0, cmd_len = strlen(cmd->args[0]); i < path->num; i++) {
        curr = path->paths[i];  /* current path */
        int p_len = strlen(curr);

        full_path = malloc(p_len + cmd_len + 2); /* initialize appended string */
        if (full_path == NULL) {
            /* malloc failed */
            return NULL;
        }

        full_path[0] = '\0';

        strncat(full_path, curr, p_len); /* copy current path */

        full_path[p_len] = '/'; /* add '/' before appending command */
        full_path[p_len + 1] = '\0';

        strncat(full_path, cmd->args[0], cmd_len); /* append command */

        if (access(full_path, X_OK) == 0) {
            /* command is executable */
            return full_path;
        }

        free(full_path);
    }

    return NULL;
}
