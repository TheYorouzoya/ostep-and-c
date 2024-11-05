#include <stdio.h>

int main(int argc, char *argv[]) {

    int *badptr = NULL;

    printf("Bad dereference: %d", *badptr);

    return 0;
}
