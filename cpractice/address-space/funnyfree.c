#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    int *arr = (int *) malloc(sizeof(int) * 10);

    for (int i = 0; i < 10; i++) {
        arr[i] = 99;
    }

    free(&arr[5]);

    printf("passed bad argument to free\n");
    return 0;
}
