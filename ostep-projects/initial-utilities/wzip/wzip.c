#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("wzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }

    int counter = 0;
    char current, previous;
    bool flag = true;

    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wzip: cannot open file\n");
            exit(EXIT_FAILURE);
        }

        while ((current = fgetc(fp)) != EOF) {
            if (flag) {
                previous = current;
                flag = false;
            }

            if (current == previous) {
                counter++;
                continue;
            }

            fwrite(&counter, sizeof counter, 1, stdout);
            fwrite(&previous, sizeof previous, 1, stdout);
            previous = current;
            counter = 1;
        }

        fclose(fp);

    }

    fwrite(&counter, sizeof counter, 1, stdout);
    fwrite(&previous, sizeof previous, 1, stdout);

    return EXIT_SUCCESS;
}
