#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Usage: ./memory_user [memory size]\n");
        exit(EXIT_FAILURE);
    }

    char *endptr;

    errno = 0;

    int memory = (int) strtol(argv[1], &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == argv[1]) {
        printf("No digits were found\n");
        exit(EXIT_FAILURE);
    }

    int *arr = (int *) malloc(sizeof(int) * memory);

    printf("pid: %d\n", (int) getpid());

    // simply stall the program and touch each array element so we can check
    // memory usage using free in the other terminal
    for(;;) {
        for (int i = 0; i < memory; i++) {
            arr[i] = 0;
        }
    }

    return 0;
}
