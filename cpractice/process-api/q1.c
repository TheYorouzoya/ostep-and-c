/*
 * Write a program that calls fork(). Before calling fork(), have the main process
 * access a variable (e.g., x) and set its value to something (e.g., 100). What
 * value is the variable in the child process? What happens to the variable when
 * both the child and parent change the value of x?
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int x = 100;

    pid_t child_p = fork();

    if (child_p == -1) {    // fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_p == 0) {     // child process returns 0 for fork()
        printf("Value for x in child: %d\n", x);
        x = 101;
        printf("New value for x in child: %d\n", x);
    } else {				// parent process
        printf("Value for x in parent: %d\n", x);
        x = 102;
        printf("New value for x in parent: %d\n", x);
    }

    return 0;
}
