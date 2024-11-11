#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./strtk [String] [Delimiter]\n");
        exit(EXIT_FAILURE);
    }

    char *line = argv[1];
    char *delim = argv[2];

    char *token, *saveptr;
    size_t count = 0;

    token = strtok_r(line, delim, &saveptr);

    if (token == NULL) {
        printf("No tokens found\n");
    }

    printf("First Token: %s\n", token);

    while ((token = strtok_r(NULL, delim, &saveptr)) != NULL) {
        printf("Loop run: %ld\tToken: %s\n", count++, token);
    }

    return EXIT_SUCCESS;
}
