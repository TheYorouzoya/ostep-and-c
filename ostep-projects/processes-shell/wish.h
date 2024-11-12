#ifndef WISH_H_
#define WISH_H_

/* Library Headers */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* External headers */
#include "path.h"
#include "commands.h"
#include "error.h"
#include "structs.h"
#include "tokens.h"

void run_shell(FILE*, int);

#endif // WISH_H_
