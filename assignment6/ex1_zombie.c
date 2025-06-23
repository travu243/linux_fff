#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        printf("Child process (PID: %d) exiting...\n", getpid());
        exit(0);
    } else {
        // parent process
        printf("Parent process (PID: %d) sleeping...\n", getpid());
        sleep(20);
    }

    return 0;
}
