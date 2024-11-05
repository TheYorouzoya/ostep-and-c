#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    // malloc 10 integers, forget to free before exit
    int *arr = (int *) malloc (sizeof(int) * 100);

    arr[100] = 0;

    printf("Exiting without calling free.\n");

    return 0;
}
