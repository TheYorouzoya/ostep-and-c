#include "path.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

path_t *init_default_path() {
    /* default path includes only '/bin' */
    path_t *p;
    p = malloc(sizeof(path_t));

    if (!p) {
        /* malloc failed */
        return NULL;
    }

    p->paths = malloc(sizeof(char **));

    if(!p->paths) {
        /* malloc failed */
        return NULL;
    }

    p->paths[0] = DEFAULT_PATHS;
    p->num = DEFAULT_PATHS_NUM;

    return p;
}

void free_path(path_t *p) {
    if (!p) return;             /* return if already null */

    if (p->paths && p->num > 0) {             /* only free initialized paths */
        char* tmp;
        for (int i = 0; i < p->num; i++) {
            tmp = *p->paths + i;
            free(tmp);
        }
        free(p->paths);
    }

    free(p);
}

size_t add_paths(path_t *path, char ***paths, const size_t path_num) {
    path_t *p;                  /* temporary path */

    /* initialize new path */
    p = malloc(sizeof(path_t));
    if (!p) {
        /* malloc failed */
        return -1;
    }

    p->num = path_num;

    if (p->num) {
        /* new path is empty */
        free_path(path);
        path = p;
        return p->num;
    }

    p->paths = malloc(sizeof(p->paths) * p->num);
    if (!p->paths) {
        /* malloc failed */
        return -1;
    }

    /* copy all path strings */
    char *tmp, *cpy;
    char **curr;                /* current path token */
    int i = 0;
    bool m_fail = false;

    curr = *paths;

    for(; i < p->num; i++) {
        curr = curr + i;
        tmp = *curr;
        cpy = malloc(sizeof(cpy) * (strlen(tmp) + 1));

        if (!cpy) {
            /* malloc failed */
            m_fail = true;
            break;
        }

        strcpy(cpy, tmp);

        p->paths[i] = cpy;
    }

    if (m_fail) {
        /* malloc failed while copying strings */
        p->num = i;
        free_path(p);
        return -1;
    }

    free_path(path);            /* adding path always overwrites old path */
    path = p;

    return path->num;
}
