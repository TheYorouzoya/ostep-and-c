#ifndef PATH_H_
#define PATH_H_

#include <stdlib.h>

#define DEFAULT_PATHS "/bin"
#define DEFAULT_PATHS_NUM 1

typedef struct path_s path_t;

struct path_s {
    char **paths;
    size_t num;
};

path_t* init_default_path();

void free_path(path_t*);

size_t add_paths(path_t*, char***, const size_t);

#endif // PATH_H_
