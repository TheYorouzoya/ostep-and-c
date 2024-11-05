#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc <= 1) exit(EXIT_SUCCESS);

    const int BUFFER_LENGTH = 256;
    char buffer[BUFFER_LENGTH];

    // for each fiel in the given arglist
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wcat: cannot open file\n");
            exit(EXIT_FAILURE);
        }
        while (fgets(buffer, BUFFER_LENGTH, fp)) {
            printf("%s", buffer);
        }
        if (fclose(fp) != 0) {
            printf("wcat: cannot close file\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
