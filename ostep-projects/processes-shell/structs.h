#ifndef STRUCTS_H_
#define STRUCTS_H_

#include<stdlib.h>

typedef struct {
    char **tokens;
    size_t num;
} tokens_t;

/**
 * Struct holds the number of paths and all path strings.
 **/
typedef struct {
    char **paths;               /* the path pointer array */
    size_t num;                 /* number of paths in array */
} path_t;

/**
 * Enum defines the different command types the shell supports
 **/
typedef enum {
    CMD_EXIT,                   /* exits shell */
    CMD_CD,                     /* changes working directory */
    CMD_PATH,                   /* changes shell path variable */
    CMD_EXTERNAL                /* external command */
} command_type;


/**
 * Struct defines a shell command with command type, the argument strings
 * associated with it, and the number of arguments themselves.
 *
 * For external commands, the command itself is included at the first position
 * within the arguments.
 **/
typedef struct {
    command_type typ;           /* the type of command: external or internal */
    size_t argc;                /* the number of args */
    char **args;                /* the args themselves */
} command_t;


#endif // STRUCTS_H_
