/*
** A simple Unix shell. The shell can be invoked with either no arguments
** or a single argument; anything else is an error.
** Features:
**
** - Interactive mode: Passing no arguments starts the shell in interactive
**                     mode. The shell waits on the user to input commands
**                     directly.
**
** - Batch mode: The shell reads input from a batch file and executes
**               commands from therein. Only a single .txt file in the current
**               working directory may be passed as an argument.
**
** - Paths: The user must specify a 'path' variable to describe the set of
**          directories to search for executables.
**
** - Redirection: The user can use the '>' character to redirect output from
**                a command to a specified file. The exact format of
**                redirection is [command (args ...) > filename]. Multiple
**                redirection operators or multiple files to the right of the
**                redirection sign are errors.
**                The redirection for build-in commands isn't full supported.
**
** - Parallel Commands: The shell also allows the user to launch parallel commands.
**                      This is accomplished with the ampersand operator as follows:
**                      ```
**                      wish> cmd1 & cmd2 args1 args1 & cmd3 args1
**                      ```
**                      The shell will run all three commands in parallel and wait
**                      on them to finish.
**
** - Program Errors: The shell only ever prints this one error message (as specified
**                   under project specifications):
**                   "An error has occured\n"
**                   Any errors thrown by a particular command will be printed as is.
**
** - Built-in Commands: The shell supports the following three build-in commands:
**                      'exit': Exits the shell.
**                      'cd': Always takes one argument (0 or >1 args are considered
**                            as an error). Allows the user to change the shell's
**                            current working directory.
**                      'path': Takes 0 or more arguments, with each argument separated
**                              with whitespace from the others. A typical usage would
**                              be like this: `wish> path /bin /usr/bin`, which would
**                              add `/bin` and `/usr/bin` to the search path of the
**                              shell. If the user sets `path` to be empty (i.e., no
**                              arguments were passed), then the shell would not be
**                              able to run any programs except the built-in commands.
**                              The `path` commands always overwrites the old path with
**                              the newly specififed path.
*/

#include "wish.h"
#include "structs.h"

#define PROMPT "wish> "

int main(int argc, char *argv[]) {
    /* shell accepts at most one argument */
    if (argc > 2) {
        printf("Usage: ./wish [argument]\n");
        exit(EXIT_FAILURE);
    }

    bool batch_mode = (argc == 2) ? true : false;

    if (batch_mode) {
        run_batch_mode();
    } else {
        run_interactive_mode();
    }

    return EXIT_SUCCESS;
}

void run_batch_mode() {
    printf("Now running in batch mode\n");
}


void run_interactive_mode() {
    bool exitFlag = false;
    char *lineptr = NULL;
    size_t n = 0, line_size;
    path_t *path = init_default_path();

    /* print the prompt and wait for user input */
    while (!exitFlag) {
        fprintf(stdout, PROMPT);

        errno = 0;

        if((line_size = getline(&lineptr, &n, stdin)) == -1) {
            if (errno == 0) {
                if (lineptr != NULL) free(lineptr);
                if(path != NULL) free_path(path);
                printf("\n");
                /* encountered EOF */
                exit(EXIT_SUCCESS);
            }
            print_error();
            exit(EXIT_FAILURE);
        }

        /* lineptr now stores the user input */
        if ((lineptr)[line_size - 1] == '\n') {
            /* remove trailing newline from getline */
            (lineptr)[line_size - 1] = '\0';
        }


        /* Even though lineptr is guaranteed to be null terminated, we'll still
         * err on the side of caution here */
        char *linecpy = (char *) malloc(sizeof(char) * (line_size + 1));
        strncpy(linecpy, lineptr, line_size); /* copy line */
        linecpy[line_size - 1] = '\0';        /* null terminate line to be sure */

        char *redir_file;
        int has_redirect = parse_redirect(linecpy, line_size, &redir_file);
        command_t *cmd = get_command(linecpy, line_size);

        if (check_syntax(cmd, has_redirect, &redir_file)) {
            if (has_redirect == 0) {
                /* command has valid redirect syntax */
                execute_command(cmd, &path);
            } else {
                /* only external commands support redirection */
                if (cmd->typ == CMD_EXTERNAL) {
                    execute_command_and_redirect(cmd, path, redir_file);
                } else {
                    /* redirection on internal commands is an error */
                    print_error();
                }
            }
        } else {
            print_error();
        }

        free(linecpy);
        free_command(cmd);
    }

    free(lineptr);
}



bool check_syntax(command_t *cmd, int has_redirect, char **redir_file) {
    if (has_redirect == -1) return false; /* if more than one redirect character present */
    if (cmd == NULL) return false;        /* if command has no valid tokens */
    return true;
}


int parse_redirect(char *lineptr, size_t line_size, char **redir_ptr) {
    /* redirection syntax is as follows:
     * command [args ...] > file */

    char *token, *saveptr;

    bool contains = (strchr(lineptr, '>') != NULL);

    token = strtok_r(lineptr, ">", &saveptr);

    if (token == NULL) {
        /* string is empty */
        return -1;
    }

    /* line is non-empty */

    if ((token = strtok_r(NULL, ">", &saveptr)) == NULL) {
        if(contains) {
            /* operator present, but file string is empty */
            return -1;
        }
        /* no redirection operator present */
        return 0;
    }

    *redir_ptr = token;         /* store file string pointer */

    if (strtok_r(NULL, ">", &saveptr) != NULL) {
        /* more than one redirect character present */
        *redir_ptr = NULL;
        return -1;
    }

    /* check if there are multiple files in the obtained token string */
    token = strtok_r(*redir_ptr, WHITESPACE, &saveptr);

    if (token == NULL) {
        /* file string token is all whitespace */
        return -1;
    }

    /* file string no longer has any leading whitespace */

    *redir_ptr = token;         /* update file string pointer position */

    /* check if file string has more than one file */
    if ((token = strtok_r(NULL, WHITESPACE, &saveptr)) != NULL) {
        /* extra files found */
        return -1;
    }

    /* if no extra files were found, the call to strtok_r() would have cleaned up any
     * trailing whitespace as well by replacing them with the null byte ('\0') */

    return 1;
}


command_t *tokenize_line(char **lineptr, const size_t line_size) {

    return NULL;
}


void execute_command(command_t *cmd, path_t **path) {
    if (cmd->typ == CMD_EXTERNAL) {
        execute_external_command(cmd, *path);
    } else {
        execute_built_in_command(cmd, path);
    }
}
