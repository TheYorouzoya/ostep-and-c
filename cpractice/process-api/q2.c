/*
 * Write a program that opens a file (with the open() system call) and then calls
 * fork() to create a new process. Can both the child and parent access the file
 * descriptor returned by open()? What happend when tehy are writing to the file
 * concurrently, i.e., at the same time?
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    int f_descriptor = open("q1.c", O_RDONLY);

    if (f_descriptor == -1) { // open failed
        perror("open");
        exit(EXIT_FAILURE);
    }

    pid_t child_p = fork();

    if (child_p == -1) { // fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }

    char buf[256];

    if (child_p == 0) {
        // inside child process
        printf("child file descriptor value is: %d\n", f_descriptor);

        if(read(f_descriptor, buf, 250) == -1) {
            // read failed
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("bytes read by the child: %s\n", buf);
        printf("child closes file descriptor\n");
        close(f_descriptor);

    } else {
        int w = wait(NULL);
        if (w == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        // inside parent process
        printf("parent file descriptor value is: %d\n", f_descriptor);

        if(read(f_descriptor, buf, 250) == -1) {
            // read failed
            perror("read");
            exit(EXIT_FAILURE);
        }

    }
    return 0;
}
