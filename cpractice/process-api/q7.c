/*
** Write a program that creates two children, and connects the standard
** output of one to the standard input of the other using the pipe()
** system call.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pipe1[2];

    /* fetch both pipes for the processes */
    if (pipe(pipe1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* Create child */
    pid_t p1 = fork();
    if (p1 == -1) {
        /* fork failed */
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (p1 == 0) {
        /* inside first child */
        printf("child1 pid: %d", (int) getpid());
        if (dup2(pipe1[0], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        puts("This message is from child 1\n");
    } else {
        pid_t p2 = fork();
        if (p2 == -1) {
            /* fork failed */
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (p2 == 0) {
            /* inside second child */
            if(dup2(pipe1[1], STDIN_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }

            printf("child2 pid: %d", (int) getpid());

            char buf[256];
            int fd = read(pipe1[1], &buf, sizeof buf);

            if (fd == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            printf("What was read from p1: %s", buf);
        } else {
            printf("parent id: " (int)getpid());
            waitpid(p1, &p1, 0);
            waitpid(p2, &p2, 0);

        }

    }

    return EXIT_SUCCESS;
}
