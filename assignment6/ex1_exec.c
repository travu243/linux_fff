#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // child process
        printf("Child process (PID: %d) executing ls -l\n", getpid());

        // replace child process with /bin/ls
        execl("/bin/ls", "ls", "-l", (char *)NULL);

        // If fail, go here
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        printf("parent process pid: %d, created child pid: %d\n", getpid(), pid);
        wait(NULL);
        printf("child done\n");
    }

    return 0;
}
