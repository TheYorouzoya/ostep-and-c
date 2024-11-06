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
    size_t n = 0, line_size, token_num;
    path_t *path;

    path = init_default_path();

    /* print the prompt and wait for user input */
    while (!exitFlag) {
        fprintf(stdout, PROMPT);

        errno = 0;

        if((line_size = getline(&lineptr, &n, stdin)) == -1) {
            if (errno == 0) {
                if (lineptr) free(lineptr);
                free_path(path);
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

        char **tokens;

        /* Even though lineptr is guaranteed to be null terminated, we'll still
         * err on the side of caution here */
        char *linecpy = (char *) malloc(sizeof(char) * (line_size + 1));
        strcpy(linecpy, lineptr);		// copy line
        linecpy[line_size - 1] = '\0';	// null terminate again to be sure

        token_num = tokenize_line(&tokens, &lineptr, line_size);

        if (token_num == -1) {
            /* no tokens found */
            print_error();
            continue;
        }

        execute_command(&tokens, token_num, path);

        free(linecpy);
        free(tokens);
    }

    free(lineptr);
}


size_t tokenize_line(char ***tokens, char **lineptr, const size_t line_size) {

    if (!*tokens) {
        return -1;
    }

    char *token, *saveptr;
    char **tmp;                 /* pointer to traverse tokens */
    size_t num = 0;

    /* start tokenizing */
    token = strtok_r(*lineptr, " ", &saveptr);	/* fetches command */

    if (token == NULL) {
        /* no tokens found i.e., line is all whitespace */
        return -1;
    }

    num++;

    *tokens = (char **) malloc(sizeof(char *) * num);

    if (!*tokens) {
        /* malloc failed */
        print_error();
        exit(EXIT_FAILURE);
    }

    /* store first token */
    tmp = *tokens;
    *tmp = token;

    /* parse the rest of the tokens */
    while ((token = strtok_r(NULL, " ", &saveptr))) {
        num++;

        *tokens = (char **) realloc(*tokens, sizeof(char *) * num);
        if (!*tokens) {
            /* realloc failed */
            return -1;
        }

        /* store token in array */
        tmp = *tokens;
        tmp = tmp + num - 1;;
        *tmp = token;
    }

    return num;
}


void execute_command(char ***tokens, size_t token_num, path_t *path) {
    char *command = *tokens[0];
    command_t cmd_type = get_command_type(command);

    if (cmd_type == CMD_EXTERNAL) {
        execute_external_command(tokens, token_num, path);
    } else {
        execute_built_in_command(cmd_type, tokens, token_num, path);
    }
}
