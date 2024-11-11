#include "path.h"
#include "structs.h"

path_t *init_default_path() {
    /* default path includes only '/bin' */
    path_t *p;
    p = malloc(sizeof(path_t));

    if (p == NULL) {
        /* malloc failed */
        return NULL;
    }

    char *def_p = "/bin";
    size_t def_len = strlen(def_p);
    size_t def_p_num = 1;

    p->paths = malloc(sizeof(char **));

    if(p->paths == NULL) {
        /* malloc failed */
        return NULL;
    }

    char *def_path_string = malloc(def_len + 1);

    if (def_path_string == NULL) {
        /* malloc failed */
        return NULL;
    }

    strncpy(def_path_string, def_p, def_len + 1);

    *p->paths = def_path_string;
    p->num = def_p_num;

    return p;
}

void free_path(path_t *p) {
    if (p == NULL) return;             /* return if already null */

    if (p->paths != NULL) {             /* only free initialized paths */
        for (int i = 0; i < p->num; i++) {
            if (p->paths[i] == NULL) continue;
            free(p->paths[i]);
        }
        free(p->paths);
    }
    free(p);
    return;
}

path_t *add_paths(command_t *cmd) {
    /* create temporary path */
    path_t *new_p;

    new_p = malloc(sizeof(path_t));
    if (new_p == NULL) {
        /* malloc failed */
        return NULL;
    }

    new_p->num = 0;


    /* copy paths to this new path */
    new_p->num = cmd->argc;

    if (new_p->num == 0 || cmd->args == NULL) {
        new_p->paths = NULL;
        return new_p;
    }

    new_p->paths = malloc(sizeof(char*) * cmd->argc);

    bool m_fail = false;
    int i = 0;

    for (char *curr = *cmd->args; i < new_p->num; i++, curr++) {
        char *cpy = malloc(sizeof(char) * (strlen(curr) + 1));

        if (cpy == NULL) {
            /* malloc failed */
            m_fail = true;
            break;
        }

        strncpy(cpy, curr, (strlen(curr) + 1));
        new_p->paths[i] = cpy;
    }

    if (m_fail) {
        /* malloc failed in loop */
        new_p->num = i;
        free_path(new_p);
        return NULL;
    }

    return new_p;
}


/* print a given path variable's contents for debugging */
void print_paths(path_t *path) {
    printf("Path num: %ld\nPaths: ", path->num);
    for (int i = 0; i < path->num; i++) {
        printf("%s ", path->paths[i]);
    }
    printf("\n");
}
