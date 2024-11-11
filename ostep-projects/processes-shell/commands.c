#include "commands.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>

command_type get_command_type(char *command) {
    if(strcmp(command, "exit") == 0) return CMD_EXIT;
    if(strcmp(command, "cd") == 0) return CMD_CD;
    if(strcmp(command, "path") == 0) return CMD_PATH;

    return CMD_EXTERNAL;
}


command_t *get_command(char *line, size_t line_len) {
    command_t *cmd = malloc(sizeof(command_t));

    if (cmd == NULL) {
        /* malloc failed */
        return NULL;
    }

    cmd->argc = 0;
    cmd->args = NULL;


    char *token, *saveptr, *tmp;

    token = strtok_r(line, WHITESPACE, &saveptr);

    if (token == NULL) {
        /* no commands found */
        return NULL;
    }

    cmd->typ = get_command_type(token);

    if (cmd->typ == CMD_EXTERNAL) {
        /* external commands need command name included in args */
        cmd->args = malloc(sizeof(cmd->args));

        if (cmd->args == NULL) {
            /* malloc failed; cleanup and return */
            free(cmd);
            return NULL;
        }

        tmp = malloc(sizeof(char) * (strlen(token) + 1));

        if (tmp == NULL) {
            /* malloc failed; cleanup and return */
            free(cmd->args);
            free(cmd);
            return NULL;
        }

        strncpy(tmp, token, strlen(token) + 1);

        cmd->args[cmd->argc] = tmp;
        cmd->argc++;
    }

    bool m_fail = false;

    /* parse rest of the args */
    while ((token = strtok_r(NULL, WHITESPACE, &saveptr)) != NULL) {
        tmp = malloc(sizeof(char) * (strlen(token) + 1));

        if (tmp == NULL) {
            /* malloc failed */
            m_fail = true;
            break;
        }

        strncpy(tmp, token, strlen(token) + 1);

        char** re_tmp = realloc(cmd->args, sizeof(char *) * (cmd->argc + 1));

        if (re_tmp == NULL) {
            /* realloc failed */
            m_fail = true;
            break;
        }

        cmd->args = re_tmp;
        cmd->args[cmd->argc] = tmp;
        cmd->argc++;
    }

    if (m_fail) {
        /* malloc failed in loop; cleanup and return */
        free_command(cmd);
        return NULL;
    }

    if (cmd->typ == CMD_EXTERNAL) {
        /* pad null arg for execv() */
        char **re_tmp = realloc(cmd->args, sizeof(char *) * (cmd->argc + 1));

        if (re_tmp == NULL) {
            /* realloc failed; cleanup and return */
            free_command(cmd);
            return NULL;
        }

        cmd->args = re_tmp;
        cmd->args[cmd->argc] = NULL;

    }

    return cmd;
}


void free_command(command_t *cmd) {
    if (cmd == NULL) return;

    if (cmd->args != NULL) {
        for (int i = 0; i < cmd->argc; i++) {
            if (cmd->args[i] != NULL) free(cmd->args[i]);
        }
        free(cmd->args);
    }

    free(cmd);
    return;
}


void print_command(command_t *cmd) {
    printf("Command type: %d\targc: %ld\nargs: ", cmd->typ, cmd->argc);
    for (int i = 0; i < cmd->argc; i++) {
        printf("%s ", cmd->args[i]);
    }
    printf("\n");
}


void execute_built_in_command(command_t *cmd, path_t **path) {
    switch(cmd->typ) {
        case CMD_EXIT:
            if (cmd->argc > 0) {
                print_error();
                return;
            }
            exit(EXIT_SUCCESS);

        case CMD_CD:
            /* process change directory (cd) command */
            if (cmd->argc != 1) {
                print_error();
                return;
            }
            execute_cd(cmd->args[0]);
            break;

        case CMD_PATH:
            /* process built-in path ocmmand */
            *path = execute_path(cmd, path);
            break;

        default:
            /* we should never reach here */
            print_error();
            break;
    }
}


void execute_cd(char *directory) {
    if (chdir(directory) == 0) {
        return;
    }

    /* cd failed */
    print_error();
}


path_t *execute_path(command_t *cmd, path_t **p) {
    path_t *new_p = add_paths(cmd);

    if (new_p == NULL) {
        /* couldn't add new path; return previous path */
        print_error();
        return *p;
    }

    free_path(*p);
    return new_p;
}
