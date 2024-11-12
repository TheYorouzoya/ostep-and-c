#include "commands.h"
#include "tokens.h"

command_type get_command_type(char *command) {
    if(strcmp(command, "exit") == 0) return CMD_EXIT;
    if(strcmp(command, "cd") == 0) return CMD_CD;
    if(strcmp(command, "path") == 0) return CMD_PATH;

    return CMD_EXTERNAL;
}


command_t *get_command(char *line) {
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
        free(cmd);
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


void execute_command(char *cmd_string, path_t **path) {
    if (cmd_string == NULL || path == NULL) {
        print_error();
        return;
    }

    char *redir_file;
    int has_redirect = parse_redirect(cmd_string, &redir_file);

    if (has_redirect == -1) {
        print_error();
        return;
    }

    command_t *cmd = get_command(cmd_string);

    if (cmd == NULL) {
        return;
    }

    if (has_redirect == 0) {
        /* command has no redirect */
        redir_file = NULL;
    }

    if (cmd->typ != CMD_EXTERNAL) {
        execute_built_in_command(cmd, path);
    } else {
        /* execute external command in child process */
        int wstatus;
        pid_t child_proc = fork();

        if (child_proc == -1) {
            /* fork failed */
            print_error();
            free_command(cmd);
            return;
        }

        if (child_proc == 0) {
            /* execute external command in child */
            execute_external_command(cmd, *path, redir_file);

            /* returning here means execv() failed */
            print_error();
            free_command(cmd);
            exit(EXIT_FAILURE);
        } else {
            /* parent simply waits for the child to finish */
            waitpid(child_proc, &wstatus, 0);
        }
    }

    free_command(cmd);
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



void execute_external_command(command_t *cmd, path_t *path, char *redir_file) {
    /* check if command is executable */
    char *exec_path = isExecutable(cmd, path);

    if (exec_path == NULL) {
        /* command not executable */
        return;
    }


    if (redir_file != NULL) {
        /* redirect stdout to file */
        FILE *fp = freopen(redir_file, "w+", stdout);

        if (fp == NULL) {
            return;
        }

        /* redirect stdee to stdout, i.e., to the file a */
        if (dup2(fileno(stdout), fileno(stderr)) == -1) {
            return;
        }
    }

    /* execute command */
    execv(exec_path, cmd->args);

    /* returning here is an error */
    free(exec_path);
}


void execute_commands_in_parallel(tokens_t *tkn, path_t **path) {
    if (tkn == NULL) {
        /* given tokens variable is empty */
        print_error();
        return;
    }

    pid_t pids[tkn->num];
    int wstatus[tkn->num];
    size_t count = 0;

    for (int i = 0; i < tkn->num; i++) {
        pid_t p_fork = fork();

        if (p_fork == -1) {
            /* fork failed */
            print_error();
            continue;
        }

        if (p_fork == 0) {
            /* execute command in child */
            char *redir_file;
            int has_redirect = parse_redirect(tkn->tokens[i], &redir_file);

            if (has_redirect == -1) {
                /* incorrect redirect syntax */
                print_error();
                exit(EXIT_FAILURE);
            }

            command_t *cmd = get_command(tkn->tokens[i]);

            if (cmd == NULL) {
                /* could not extract a command from string token */
                print_error();
                exit(EXIT_FAILURE);
            }

            if (has_redirect == 0) {
                /* command has no redirect */
                redir_file = NULL;
            }

            if (cmd->typ != CMD_EXTERNAL) {
                /* command is a built-in command */
                execute_built_in_command(cmd, path);
            } else {
                execute_external_command(cmd, *path, redir_file);

                /* returning here means execv() failed */
                print_error();
                free_command(cmd);
                exit(EXIT_FAILURE);
            }

            free_command(cmd);
            exit(EXIT_SUCCESS);

        } else {
            /* parent stores the process ID and executes next command */
            pids[count++] = p_fork;
        }
    }

    /* only parent will return here */
    for (int i = 0; i < count; i++) {
        /* wait for all childs to terminate */
        waitpid(pids[i], &wstatus[i], 0);
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
