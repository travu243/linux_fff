#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;

    // create child process 1
    pid1 = fork();

    if (pid1 < 0) {
        perror("fork 1 failed");
        exit(1);
    } else if (pid1 == 0) {
        // child process 1
        printf("child 1: pid = %d, parent pid = %d\n", getpid(), getppid());
        exit(0);
    }

    // create child process 2
    pid2 = fork();

    if (pid2 < 0) {
        perror("fork 2 failed");
        exit(1);
    } else if (pid2 == 0) {
        // child process 2
        printf("child 2: pid = %d, parent pid = %d\n", getpid(), getppid());
        exit(0);
    }

    // Parent
    printf("parent: pid = %d\n", getpid());

    wait(NULL);
    wait(NULL);

    return 0;
}
