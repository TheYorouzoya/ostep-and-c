/*
** Now write a program that uses wait() to wait for the child process to
** finish in the parent. What does wait() return? What happens if you use
** wait() in the child?
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t proc = fork();

    if (proc == -1) {
        // fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (proc == 0) {
        // inside child
        printf("Child process started! Waiting...\n");
        wait(NULL);

    } else {
        // inside parent
        printf("Parent process started.\n");
    }
    return 0;
}
