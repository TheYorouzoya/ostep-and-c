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

#define PROMPT "wish> "
#define BATCH_MODE 1
#define INTERACTIVE_MODE 0

int main(int argc, char *argv[]) {
    /* shell accepts at most one argument */
    if (argc > 2) {
        print_error();
        exit(EXIT_FAILURE);
    }

    int mode = (argc == 2) ? BATCH_MODE : INTERACTIVE_MODE;

    FILE *input_stream;

    if (mode == BATCH_MODE) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            print_error();
            exit(EXIT_FAILURE);
        }
    } else {
        input_stream = stdin;
    }

    run_shell(input_stream, mode);

    if (mode == BATCH_MODE) fclose(input_stream);

    return EXIT_SUCCESS;
}


void run_shell(FILE *input_stream, int mode) {
    char *lineptr = NULL;
    size_t n = 0, line_size;
    path_t *path = init_default_path();

    /* print the prompt and wait for user input */
    while (true) {

        if (mode == INTERACTIVE_MODE) fprintf(stdout, PROMPT);

        errno = 0;

        if((line_size = getline(&lineptr, &n, input_stream)) == -1) {
            if (errno == 0) {
                if (lineptr != NULL) free(lineptr);
                if(path != NULL) free_path(path);
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

        tokens_t *tokens = tokenize_line(&linecpy, line_size);

        if (tokens == NULL) {
            /* no valid tokens in line */
            free(linecpy);
            continue;
        }

        if (tokens->num == 1) {
            /* execute the single command */
            execute_command(linecpy, &path);
        } else {
            execute_commands_in_parallel(tokens, &path);
        }


        free(linecpy);
        free_tokens(tokens);
    }

    free(lineptr);
}
