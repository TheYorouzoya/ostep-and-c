#ifndef STRUCTS_H_
#define STRUCTS_H_

#include<stdlib.h>

/**
 * Struct holds the number of paths and all path strings
 **/
struct path_s {
    char **paths;
    size_t num;
};

typedef enum {
    CMD_EXIT,                   /* exits shell */
    CMD_CD,                     /* changes working directory */
    CMD_PATH,                   /* changes shell path variable */
    CMD_EXTERNAL                /* external command */
} command_type;


struct command {
    command_type typ;           /* the type of command: external or internal */
    size_t argc;                /* the number of args */
    char **args;                /* the args themselves */
};


typedef struct command command_t;

typedef struct path_s path_t;


#endif // STRUCTS_H_
