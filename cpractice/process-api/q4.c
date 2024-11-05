/*
** Write a program that calls fork() and then calls some form of exec()
** to run the program /bin/ls. See if you can try all of the variants of
** exec(), including (on Linux) execl(), execle(), execlp(), execv(), execvp(),
** and execvpe(). Why do you think there are so many variants of the same basic
** call?
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
        int ex = execl("/bin/ls", "-a", "-l", (char *) NULL);
        if (ex == -1) {
            //exec failed
            perror("exec");
            exit(EXIT_FAILURE);
        }
    } else {
        // inside parent
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
