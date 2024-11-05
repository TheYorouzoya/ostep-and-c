#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(EXIT_FAILURE);
    }

    char *query = argv[1];

    if (argc == 2) {
        const size_t BUFFER_LENGTH = 256;
        char buffer[BUFFER_LENGTH];

        while (fgets(buffer, BUFFER_LENGTH, stdin)) {
            if (strstr(buffer, query) != NULL) {
                printf("%s", buffer);
            }
        }
        exit(EXIT_SUCCESS);
    }

    for(int i = 2; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            exit(EXIT_FAILURE);
        }

        size_t n = 0;
        char *lineptr = NULL;

        while (getline(&lineptr, &n, fp) != -1) {
            if (strstr(lineptr, query) != NULL) {
                printf("%s", lineptr);
            }
        }

        free(lineptr);

        if (fclose(fp)) {
            printf("wgrep: could not close file properly\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
