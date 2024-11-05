/*
** Write a program that creates a child process, and then in the child
** closes standard output (STDOUT_FILENO). What happens if the child calls
** printf() to print some output after closing the descriptor?
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t proc = fork();

    if (proc == -1) {
        /* fork failed */
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (proc == 0) {
        /* inside child */
        close(STDOUT_FILENO);
        printf("Calling printf with stdout closed\n");
        /*
        ** Seems like nothing was printed on the output for this
        ** print statement here.
             */
    } else {
        printf("Parent just chilling\n");
        wait(NULL);
        printf("Can the parent print after child has closed stdout?\n");
        /*
        ** The parent can print just fine it seems
        */
    }

    return 0;
}
