/*
** Write another program using fork(). The child process should print "hello";
** the parent process should print "goodbye". You should try to ensure that the
** child process always prints first; can you do this without calling wait() in
** the program?
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t proc = fork();

    if (proc == -1) {	// fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (proc == 0) {
        // inside child
        printf("hello\n");
    } else {
        // inside parent
        /* pid_t wt = wait(NULL); */
        /* if (wt == -1) { */
        /*     perror("wait"); */
        /*     exit(EXIT_FAILURE); */
        /* } */
        sleep(1);
        printf("goodbye\n");
    }

    return EXIT_SUCCESS;
}
