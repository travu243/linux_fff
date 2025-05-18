#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        printf("Child: PID = %d. Exiting now.\n", getpid());
        exit(0); // Exit immediately
    } else {
        // Parent process
        printf("Parent: PID = %d. Child PID = %d\n", getpid(), pid);
        printf("Parent: Sleeping for 10 seconds...\n");
        sleep(10); // Sleep before calling wait()
        printf("Parent: Done sleeping. Exiting now.\n");
    }

    return 0;
}

