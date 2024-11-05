#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    int *arr = (int *) malloc(sizeof(int) * 10);

    for (int i = 0; i < 10; i++) {
        arr[i] = 0;
    }

    free(arr);

    printf("Referencing array after freeing: %d\n", arr[5]);

    return 0;
}
