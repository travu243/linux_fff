#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        sleep(10);
        printf("Child (PID: %d), parent now is: %d\n", getpid(), getppid());
    } else {
        // parent process
        printf("Parent (PID: %d) exiting...\n", getpid());
        exit(0);
    }

    return 0;
}
