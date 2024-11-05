#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");

        if (fp == NULL) {
            printf("wunzip: cannot open file\n");
            exit(EXIT_FAILURE);
        }

        int counter;
        char current;

        while (fread(&counter, sizeof counter, 1, fp) != 0) {
            fread(&current, sizeof current, 1, fp);
            int n = 0;
            while (n < counter) {
                printf("%c", current);
                n++;
            }
        }

        fclose(fp);
    }

    return EXIT_SUCCESS;
}
